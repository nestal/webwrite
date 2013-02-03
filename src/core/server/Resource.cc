/*
	webwrite: an GPL wiki-like website with in-place editing
	Copyright (C) 2012  Wan Wai Ho

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation version 2
	of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Resource.hh"
#include "Config.hh"
#include "parser/Json.hh"
#include "util/File.hh"
#include "log/Log.hh"

#include <boost/exception/diagnostic_information.hpp>

#include <algorithm>
#include <cassert>
#include <string>

namespace wb {

Resource::Resource() :
	m_path( cfg::Inst()["main_page"].Str() )
{
}

Resource::Resource( const std::string& uri )
{
	std::string wb_root = cfg::Inst()["wb_root"].Str() ;
	
	if ( uri.substr( 0, wb_root.size() ) != wb_root )
		BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( "invalid resource path" ) ) ;
	
	m_path = uri.substr(wb_root.size()) ;
	
	if ( Filename().empty() || Filename() == "." || fs::is_directory(DataPath()) )
		m_path /= cfg::Inst()["main_page"].Str() ; 
}

const fs::path& Resource::Path() const
{
	return m_path ;
}

std::string Resource::Filename() const
{
	return m_path.filename().string() ;
}

/// name of the resource. This string is NOT encoded in %-format (RFC1738). Assuming
/// the web server is using UTF8, this string should be in UTF8 encoding.
std::string Resource::Name() const
{
	// the file name on disk is using the URI encoding. i.e. it has %20 instead of
	// space. this way there is no need to re-encode the name when reading and writing.
	return DecodeName( Filename() ) ;
}

std::string Resource::ParentName() const
{
	return DecodeName( m_path.parent_path().filename().string() ) ;
}

std::string Resource::DecodeName( const std::string& uri )
{
	std::string result ;
	for ( std::string::const_iterator i = uri.begin() ; i != uri.end() ; ++i )
	{
		if ( *i == '%' )
		{
			std::string c = uri.substr( i-uri.begin(), 3 ) ;
			if ( c.size() == 3 )
			{
				long r = std::strtol( c.c_str()+1, 0, 16 ) ;
				if ( r >= 0 && r <= std::numeric_limits<unsigned char>::max() )
					result.push_back( static_cast<char>( r ) ) ;
				i += 2 ;
			}
		}
		else
			result.push_back( *i );
	}
	return result ;
}

fs::path Resource::DataPath() const
{
	return cfg::Inst()["data"]["path"].Str() / m_path ;
}

fs::path Resource::ReDirPath() const
{
	return cfg::Inst()["data"]["redir"].Str() / m_path ;
}

fs::path Resource::UrlPath() const
{
	return cfg::Path("wb_root") / m_path ;
}

fs::path Resource::AtticPath() const
{
	return (cfg::Inst()["attic"]["path"].Str() / m_path).parent_path() ;
}

fs::path Resource::MetaPath() const
{
	return cfg::Inst()["meta"]["path"].Str() / m_path ;
}

std::string Resource::Type() const
{
	return cfg::MimeType( Path() ) ;
}

Json Resource::Meta() const
{
	fs::path file = cfg::Inst()["meta"]["path"].Str() / m_path ;
	Json meta ;
	
	try
	{
		File in( file.string() ) ;
		meta = Json::Parse( &in ) ;
	}
	catch ( Exception& e )
	{
		boost::system::error_code ec ;

		std::time_t write_time = fs::last_write_time( DataPath(), ec ) ;
		if ( ec )
			write_time = std::time(0) ;
		
		meta.Add( "last-modified", Json(write_time) ) ;
	}
	
	return meta ;
}

void Resource::SaveMeta(std::time_t modified) const
{
	fs::path file = cfg::Inst()["meta"]["path"].Str() / m_path ;
	fs::create_directories( file.parent_path() ) ;
	
	Json meta = Meta() ;
	meta.Add( "last-modified", Json(modified) ) ;

	File out( file, 0600 ) ;
	meta.Write( &out ) ; 
}

/**	Move the resource to attic. This function will deduce the name of
	the file in attic. 
*/
void Resource::MoveToAttic() const
{
	// create the directory in the attic first
	fs::path attic = AtticPath() ;
	fs::create_directories( attic ) ;

	// use the last modification time to name the file
	Json meta = Meta() ;
	int modified = meta["last-modified"].Int() ;
	
	// move file. ignore any error
	boost::format attic_fn( "%1%-%2%" ) ;
	boost::system::error_code oops ;
	fs::path dest = attic / (attic_fn % Filename() % modified).str() ;	
	fs::rename( DataPath(), dest, oops ) ;

	Log( "moving to %1%", dest ) ;
}

} // end of namespace

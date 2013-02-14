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
#include <bitset>
#include <cassert>
#include <string>

namespace
{
	struct TruePred
	{
		template <typename T>
		bool operator()( T ) const
		{
			return true ;
		}
	} ;
	
	struct Marked
	{
		// according to RFC2396, these characters are allowed and no need to
		// be escaped
		static std::bitset<256> Map()
		{
			std::bitset<256> result ;
			result['-']  = true ;
			result['_']  = true ;
			result['.']  = true ;
			result['!']  = true ;
			result['~']  = true ;
			result['*']  = true ;
			result['\''] = true ;
			result['(']  = true ;
			result[')']  = true ;
			return result ;
		}
	
		bool operator()( char t ) const
		{
			static const std::bitset<256> map = Map() ;
			return map[static_cast<unsigned char>(t)] ;
		}
	} ;
}

namespace wb {

Resource::Resource() :
	m_path( Cfg::Inst().main_page )
{
}

Resource::Resource( const std::string& uri )
{
	const std::string& wb_root = Cfg::Inst().wb_root ;
	
	if ( uri.substr( 0, wb_root.size() ) != wb_root )
		BOOST_THROW_EXCEPTION( Error() << expt::ErrMsg( "invalid resource path" ) ) ;
	
	// decode the marked characters. firefox sometimes encoded them in % format.
	m_path = DecodePercent( uri.substr(wb_root.size()), Marked() ) ;
	
	if ( Filename().empty() || Filename() == "." || fs::is_directory(DataPath()) )
		m_path /= Cfg::Inst().main_page ; 
}

bool Resource::CheckRedir( const std::string& uri ) const
{
	return UrlPath() != DecodePercent( uri, Marked() ) ;
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
	std::string name = DecodeName( Filename() ) ;
	std::replace( name.begin(), name.end(), '_', ' ' ) ;
	return name ;
}

std::string Resource::ParentName() const
{
	return DecodeName( m_path.parent_path().filename().string() ) ;
}

std::string Resource::DecodeName( const std::string& uri )
{
	return DecodePercent( uri, TruePred() ) ;
}

template <typename Pred>
std::string Resource::DecodePercent( const std::string& uri, Pred pred )
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
				{
					if ( r == ' ' )
						result.push_back( '_' ) ;
					else if ( pred( static_cast<char>(r) ) )
						result.push_back( static_cast<char>(r) ) ;
					else
						result.insert( result.end(), c.begin(), c.end() ) ;
				}
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
	return Cfg::Inst().data.path / m_path ;
}

fs::path Resource::ReDirPath() const
{
	return Cfg::Inst().data.redir / m_path ;
}

fs::path Resource::UrlPath() const
{
	return Cfg::Inst().wb_root / m_path ;
}

fs::path Resource::AtticPath() const
{
	return (Cfg::Inst().attic.path / m_path).parent_path() ;
}

fs::path Resource::MetaPath() const
{
	return Cfg::Inst().meta.path / m_path ;
}

std::string Resource::Type() const
{
	return Cfg::MimeType( Path() ) ;
}

Json Resource::Meta() const
{
	fs::path file = Cfg::Inst().meta.path / m_path ;
	Json meta ;
	
	try
	{
		File in( file.string() ) ;
		meta = Json::Parse( &in ) ;
	}
	catch ( Exception& )
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
	fs::path file = Cfg::Inst().meta.path / m_path ;
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
}

} // end of namespace

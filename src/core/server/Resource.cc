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
#include "parser/UriChar.hh"
#include "parser/UriCharIterator.hh"
#include "util/File.hh"
#include "log/Log.hh"

#include <boost/exception/diagnostic_information.hpp>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <string>

namespace
{
	template <char src, char target>
	struct CharMap
	{
		char operator()( char in ) const
		{
			return in == src ? target : in ;
		}
	} ;

	struct IdentCharMap
	{
		char operator()( char in ) const
		{
			return in ;
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
	m_path = DecodePercent( uri.substr(wb_root.size()), Marked(), CharMap<' ', '_'>() ) ;
	
	if ( Filename().empty() || Filename() == "." || fs::is_directory(DataPath()) )
		m_path /= Cfg::Inst().main_page ; 
}

bool Resource::CheckRedir( const std::string& uri ) const
{
	return UrlPath() != DecodePercent( uri, Marked(), IdentCharMap() ) ;
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
	return DecodeName( Filename() ) ;
}

std::string Resource::ParentName() const
{
	return DecodeName( m_path.parent_path().filename().string() ) ;
}

std::string Resource::DecodeName( const std::string& uri )
{
	std::string out ;
	UriCharIterator<> it( uri.begin(), uri.end() ), end( uri.end(), uri.end() ) ;
	std::transform( it, end, std::back_inserter(out), CharMap<'_', ' '>() ) ;
	return out ;
}

template <typename Pred, typename CharMapT>
std::string Resource::DecodePercent( const std::string& uri, Pred pred, CharMapT cmap )
{
	std::string result ;
	UriCharIterator<> it( uri.begin(), uri.end() ), end( uri.end(), uri.end() ) ;
	for ( ; it != end ; ++it )
	{
		UriChar<> ch = *it ;
		switch ( ch.CharType() )
		{
		case UriChar<>::escape :
			if ( !pred( ch ) )
			{
				result.insert( result.end(), ch.begin(), ch.end() ) ;
				break ;
			}
			// fall-through
		
		case UriChar<>::mark :
		case UriChar<>::alphanum :
			result.push_back( cmap( ch ) ) ;
			break ;
		}
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

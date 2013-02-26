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
	using namespace wb ;

	template <char src, char target>
	struct CharMap
	{
		char operator()( const UriChar<>& uc ) const
		{
			char in = uc;
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
		BOOST_THROW_EXCEPTION( Error() << InvalidUri_( uri ) ) ;
	
	// decode the marked characters. firefox sometimes encoded them in % format.
	m_path = DecodePercent( uri.substr(wb_root.size()), CharMap<' ', '_'>() ) ;
	
	if ( Filename().empty() || Filename() == "." || fs::is_directory(DataPath()) )
		m_path /= Cfg::Inst().main_page ; 
}

Resource::~Resource()
{
}

bool Resource::CheckRedir( const std::string& uri ) const
{
	return UrlPath() != DecodePercent( uri, IdentCharMap() ) ;
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
	// deducing the name is fasting than reading it from the meta
	return m_meta.get() != 0 ? m_meta->name : DeduceName() ;
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

template <typename CharMapT>
std::string Resource::DecodePercent( const std::string& uri, CharMapT cmap )
{
	std::string result ;
	UriCharIterator<> it( uri.begin(), uri.end() ), end( uri.end(), uri.end() ) ;
	for ( ; it != end ; ++it )
	{
		UriChar<> ch = *it ;
		switch ( ch.CharType() )
		{
		case UriChar<>::escape :
			if ( !Marked()( ch ) )
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
	LoadMeta() ;
	return m_meta->type ;
}

Resource::Meta Resource::GetMeta() const
{
	LoadMeta() ;
	return *m_meta ;
}

/**	Load metadata from file (i.e. from the MetaPath()). If the meta file doesn't exist or it
	doesn't contain a particular field (e.g. no last-modified), this function will try to
	deduce the missing metadata itself.

	\note Parsing JSON and possibly deducing metadata makes this function potentially slow.
*/
void Resource::LoadMeta() const
{
	// it's good if we already loaded it
	if ( m_meta.get() != 0 )
		return ;

	// load meta from file
	m_meta.reset(new Meta) ;
	m_meta->sequence = 0 ;

	fs::path file = Cfg::Inst().meta.path / m_path ;
	try
	{
		File in( file.string() ) ;
		Json meta = Json::Parse( &in ) ;

		m_meta->modified	= meta["last-modified"].As<std::time_t>() ;
		m_meta->name		= meta["name"].Str() ;
		m_meta->type		= meta["type"].Str() ;
		m_meta->sequence	= meta["sequence"].Int() ;
	}
	catch ( Exception& )
	{
	}

	if ( m_meta->modified == 0 )
	{
		boost::system::error_code ec ;

		m_meta->modified = fs::last_write_time( DataPath(), ec ) ;
		if ( ec )
			m_meta->modified = std::time(0) ;
	}

	if ( m_meta->name.empty() )
		m_meta->name	= DeduceName() ;
	if ( m_meta->type.empty() )
		m_meta->type	= DeduceType() ;
}

void Resource::SaveMeta() const
{
	fs::path file = Cfg::Inst().meta.path / m_path ;
	fs::create_directories( file.parent_path() ) ;
	
	LoadMeta() ;
	++m_meta->sequence ;

	Json meta ;
	meta.Add( "last-modified",	Json(m_meta->modified) ) ;
	meta.Add( "name",			Json(m_meta->name) ) ;
	meta.Add( "type",			Json(m_meta->type) ) ;
	meta.Add( "sequence",		Json(m_meta->sequence) ) ;

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
	LoadMeta() ;
	
	// move file. ignore any error
	boost::format attic_fn( "%1%-%2%" ) ;
	boost::system::error_code oops ;
	fs::path dest = attic / (attic_fn % Filename() % m_meta->modified).str() ;	
	fs::rename( DataPath(), dest, oops ) ;
}

std::string Resource::DeduceType() const
{
	return Cfg::MimeType( Path() ) ;
}

std::string Resource::DeduceName() const
{
	return DecodeName( Filename() ) ;
}

} // end of namespace

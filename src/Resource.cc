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

#include "util/Exception.hh"

#include <algorithm>
#include <cassert>
#include <string>

namespace wb {

Resource::Resource( const std::string& uri )
{
	std::string wb_root = cfg::Inst()["wb_root"].Str() ;
	
	if ( uri.substr( 0, wb_root.size() ) != wb_root )
		throw Exception() ;
	
	m_path = uri.substr(wb_root.size()) ;
	
	if ( Filename().empty() || Filename() == "." || fs::is_directory(ContentPath()) )
		m_path /= cfg::Inst()["main_page"].Str() ; 
}

Resource::Resource( const fs::path& res_path ) :
	m_path( res_path )
{
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

fs::path Resource::ContentPath() const
{
	return cfg::Path("base") / cfg::Path("data_path") / m_path ;
}

fs::path Resource::UrlPath() const
{
	return cfg::Path("wb_root") / m_path ;
}

std::string Resource::Type() const
{
	return cfg::MimeType( ContentPath() ) ;
}

} // end of namespace

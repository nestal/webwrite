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

#include "RootServer.hh"

#include "Config.hh"
#include "Request.hh"

#include <iostream>

namespace wb {

RootServer::RootServer( const Config& cfg ) :
	m_file( cfg.Base() / cfg.Str("lib-path") ),
	m_data( cfg.Base() / cfg.Str("data-path") ),
	m_wb_root( cfg.Str("wb-root") )
{
}
	
Server* RootServer::Work( Request *req, const fs::path& location ) 
{
	fs::path	rel		= Relative( location ) ;
	std::string	fname	= rel.filename().string() ;

	std::size_t pos = std::string::npos ;
	
	if ( fname.empty() || fname == "." )
	{
std::cout << "wow.. fun request: " << fname << std::endl ;
		req->PrintF( "303 See Other\r\nLocation: %s\r\n\r\n",
			(location/"main").string().c_str() ) ;
		return 0 ;
	}
	
	else if ( fname.front() == '_' )
		return m_file.Work( req, fname.substr(1) ) ;
	
	else if ( rel.empty() || req->Query().empty() )
	{
		if ( fs::is_directory( m_data.LocalPath(rel) ) )
		{
			req->PrintF( "303 See Other\r\nLocation: %s\r\n\r\n",
				(location/"main").string().c_str() ) ;
			return 0 ;
		}
		else
			return m_file.Work( req, "index.html" ) ;
	}
	else
	{
		return m_data.Work( req, rel ) ;
	}
}

fs::path RootServer::Relative( const fs::path& loc ) const
{
	std::pair<fs::path::const_iterator, fs::path::const_iterator> r =
		std::mismatch( m_wb_root.begin(), m_wb_root.end(), loc.begin() ) ;

	fs::path result ;
	while ( r.second != loc.end() )
	{
		result /= *r.second ;
		r.second++ ;
	}
	
	return result ;
}

} // end of namespace

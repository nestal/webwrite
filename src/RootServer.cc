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
#include "Resource.hh"

#include <iostream>

namespace wb {

RootServer::RootServer( const Config& cfg ) :
	m_file( cfg.Base() / cfg.Str("lib-path") ),
	m_data( cfg.Base() / cfg.Str("data-path") ),
	m_wb_root( cfg.Str("wb-root") ),
	m_main_page( cfg.MainPage() )
{
}
	
Server* RootServer::Work( Request *req, const Resource& res ) 
{
	fs::path	rel		= res.Path() ;
	std::string	fname	= res.Filename() ;

	std::size_t pos = std::string::npos ;
	
	// no filename in request, redirect to main page
	if ( res.IsDir() || fname == "_" )
	{
		req->SeeOther( (res.Path()/m_main_page).string() ) ;
		return 0 ;
	}
	
	else if ( fname[0] == '_' )
		return m_file.Work( req, res ) ;
	
	else if ( rel.empty() || req->Query().empty() )
	{
		return m_file.Work( req, res ) ;
	}
	else
	{
		return m_data.Work( req, res ) ;
	}
}

} // end of namespace

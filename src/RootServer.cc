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
#include "util/File.hh"
#include "parser/FormData.hh"

#include <boost/regex.hpp>

#include <iostream>

namespace wb {

RootServer::RootServer( const Config& cfg ) :
	m_lib_path	( cfg.Base() / cfg.Str("lib-path") ),
	m_data_path	( cfg.Base() / cfg.Str("data-path") ),
	m_wb_root	( cfg.Str("wb-root") ),
	m_main_page	( cfg.MainPage() )
{
}
	
void RootServer::Work( Request *req, const Resource& res ) 
{
	fs::path	rel		= res.Path() ;
	std::string	fname	= res.Filename() ;
	
	// no filename in request, redirect to main page
	if ( res.IsDir() )
	{
		std::string path = (m_wb_root/res.Path()/m_main_page).string() ;
		if ( !req->Query().empty() )
			path += ("?" + req->Query()) ;
		
		req->SeeOther( path ) ;
	}

	else if ( !req->Query().empty() )
		return ServeContent( req, res ) ;
	
	else
	{
		std::cout << "serving home page for " << res.Path() << std::endl ;
		req->XSendFile( m_lib_path / "index.html" ) ;
	}
}

void RootServer::ServeContent( Request *req, const Resource& res )
{
	std::string	fname	= res.Filename() ;
	fs::path 	file	= res.ContentPath() ;
	
	static const boost::regex lib( "lib=(.+)" ) ;
	boost::smatch m ;

	if ( req->Method() == "POST" && req->Query() == "save" )
	{
std::cout << "writing to " << file << std::endl ;
		
		fs::create_directories( file.parent_path() ) ;
		File f( file, 0600 ) ;
		
		char buf[80] ;
		std::size_t c ;
		while ( (c = req->In()->Read(buf, sizeof(buf)) ) > 0 )
			f.Write( buf, c ) ;
	}
	
	if ( req->Method() == "POST" && req->Query() == "upload" )
	{
		FormData form( req->In(), req->ContentType() ) ;
		form.Save( res.ContentPath().parent_path() ) ;
	}
	
	else if ( req->Method() == "GET" && req->Query() == "load" )
	{
std::cout << "reading from " << file << std::endl ;
		req->XSendFile( file ) ;
	}
	
	else if ( req->Method() == "GET" && boost::regex_search( req->Query(), m, lib ) )
		ServeLibFile( req, res.Path(), m[1].str() ) ;
}

void RootServer::ServeLibFile( Request *req, const fs::path& res_path, const std::string& libfile )
{
	// only serve file if it is in the root path
	if ( res_path.parent_path() == "/" )
	{
		fs::path path = m_lib_path / libfile ;
		std::cout << "serving lib file: " << path << std::endl ;
		req->XSendFile( path ) ;
	}
	else
	{
		fs::path path = m_wb_root / ("main?lib=" + libfile) ;
		std::cout << "redirecting to: " << path << std::endl ;
		req->SeeOther( path.string() ) ;
	}
}

} // end of namespace

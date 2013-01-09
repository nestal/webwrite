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
#include "log/Log.hh"
#include "parser/FormData.hh"
#include "util/File.hh"

#include <boost/regex.hpp>

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
		Log( "serving home page for %1%", res.Path(), log::verbose ) ;
		req->XSendFile( m_lib_path / "index.html" ) ;
	}
}

void RootServer::ServeContent( Request *req, const Resource& res )
{
	std::string	fname	= res.Filename() ;
	fs::path 	file	= res.ContentPath() ;
	std::string qstr	= req->Query() ;
	
	static const boost::regex re( "lib=(.+)" ) ;
	boost::smatch m ;

	if ( req->Method() == "POST" && qstr == "save" )
	{
		Log( "writing to file %1%", file, log::verbose ) ;
		
		fs::create_directories( file.parent_path() ) ;
		File f( file, 0600 ) ;
		
		char buf[80] ;
		std::size_t c ;
		while ( (c = req->In()->Read(buf, sizeof(buf)) ) > 0 )
			f.Write( buf, c ) ;
	}
	
	if ( req->Method() == "POST" && qstr == "upload" )
	{
		FormData form( req->In(), req->ContentType() ) ;
		form.Save( res.ContentPath().parent_path() ) ;
	}
	
	else if ( req->Method() == "GET" && qstr == "load" )
	{
		Log( "reading from %1%", file, log::verbose ) ;
		req->XSendFile( fs::exists( file ) ? file : (m_lib_path / "notfound.html") ) ;
		
	}
	
	else if ( req->Method() == "GET" && qstr == "var" )
		ServeVar( req ) ;
	
	else if ( req->Method() == "GET" && qstr == "index" )
		ServeIndex( req, res ) ;
	
	else if ( req->Method() == "GET" && boost::regex_search( qstr, m, re ) )
		ServeLibFile( req, res.Path(), m[1].str() ) ;
}

void RootServer::ServeLibFile( Request *req, const fs::path& res_path, const std::string& libfile )
{
	// only serve file if it is in the root path
	if ( res_path.parent_path() == "/" )
	{
		fs::path path = m_lib_path / libfile ;
		Log( "serving lib file: %1%", path, log::verbose ) ;
		req->XSendFile( path ) ;
	}

	// request for lib file using non-root paths will get a redirect
	else
	{
		fs::path path = m_wb_root / ("main?lib=" + libfile) ;
		Log( "redirecting to: %1%", path, log::verbose );
		req->SeeOther( path.string() ) ;
	}
}

void RootServer::ServeVar( Request *req )
{
	// path to URLs should be generic strings
	Json var ;
	var.Add( "wb_root", Json( m_wb_root.generic_string() ) ) ;
	var.Add( "main", Json( m_main_page ) ) ;
		
	std::string s = var.Str() ;
	req->PrintF( "Content-type: application/json\r\n\r\n%s\r\n\r\n", s.c_str() ) ;
}

void RootServer::ServeIndex( Request *req, const Resource& res )
{
	req->PrintF( "Content-type: text/html\r\n\r\n" ) ;

	req->PrintF( "<ul>" ) ;
	fs::directory_iterator di( res.ContentPath().parent_path() ), end ;
	for ( ; di != end ; ++di )
	{
		std::string fname = di->path().filename().string() ;
		req->PrintF( "<li class=\"idx_file\">%s</li>", fname.c_str() ) ;
	}
	req->PrintF( "</ul>\r\n\r\n" ) ;
}

} // end of namespace

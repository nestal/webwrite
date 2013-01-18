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
#include <boost/bind.hpp>

namespace wb {

RootServer::RootServer( ) :
	m_lib_path	( cfg::Path("base") / cfg::Path("lib_path") ),
	m_data_path	( cfg::Path("base") / cfg::Path("data_path") ),
	m_wb_root	( cfg::Inst()["wb_root"].Str() ),
	m_main_page	( cfg::Inst()["main_page"].Str() )
{
	m_query.Add( "mimecss", &RootServer::ServeMimeCss ) ;
}
	
void RootServer::Work( Request *req, const Resource& res ) 
{
	fs::path	rel		= res.Path() ;
	
	if ( res.UrlPath() != req->SansQueryURI() )
		req->SeeOther( res.UrlPath().string(), true ) ;
	
	else if ( !req->Query().empty() )
		return ServeContent( req, res ) ;
	
	else
	{
		if ( res.Type() == "text/html" )
		{
			Log( "serving home page for %1%", res.Path(), log::verbose ) ;
			req->XSendFile( m_lib_path / "index.html" ) ;
		}
		else
			ServeDataFile( req, res ) ;
	}
}

void RootServer::ServeContent( Request *req, const Resource& res )
{
	std::string qstr	= req->Query() ;
	
	static const boost::regex re( "lib=(.+)" ) ;
	boost::smatch m ;

	if ( req->Method() == "POST" && qstr == "save" )
	{
		fs::path 	file	= res.ContentPath() ;
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
		if ( !ServeDataFile( req, res ) )
			ServeLibFile( req, res.Path(), "notfound.html" ) ;
	}

	else if ( req->Method() == "GET" && qstr == "var" )
		ServeVar( req ) ;
	
	else if ( req->Method() == "GET" && qstr == "index" )
		ServeIndex( req, res ) ;
	
	else if ( req->Method() == "GET" && boost::regex_search( qstr, m, re ) )
		ServeLibFile( req, res.Path(), m[1].str() ) ;
}

bool RootServer::ServeDataFile( Request *req, const Resource& res )
{
	fs::path file	= res.ContentPath() ;
	if ( fs::exists(file) )
	{
		Log( "reading from %1%, type %2%", file, res.Type(), log::verbose ) ;
		req->PrintF( "Content-type: %1%\r\n", res.Type() ) ;

		req->XSendFile( file ) ;
		return true ;
	}
	else
		return false ;
}

void RootServer::ServeLibFile( Request *req, const fs::path& res_path, const std::string& libfile )
{
	fs::path p( libfile ) ;

	if ( std::find( p.begin(), p.end(), ".." ) != p.end() )
		req->NotFound() ;

	// only serve file if it is in the root path
	else if ( res_path.parent_path() == "/" )
	{
		fs::path path = m_lib_path / p ;
		Log( "serving lib file: %1% %2%", path, cfg::MimeType(path), log::verbose ) ;
		
		req->PrintF( "Content-type: %1%\r\n", cfg::MimeType(path) ) ;
		req->XSendFile( path ) ;
	}

	// request for lib file using non-root paths will get a redirect
	else
	{
		std::string path = m_wb_root + "main?lib=" + libfile ;
		req->SeeOther( m_wb_root + "/" + m_main_page + "?lib=" + libfile ) ;
	}
}

void RootServer::ServeVar( Request *req )
{
	// path to URLs should be generic strings
	Json var ;
	var.Add( "name", Json( cfg::Inst()["name"] ) ) ;
	var.Add( "wb_root", Json( m_wb_root ) ) ;
	var.Add( "main", Json( m_main_page ) ) ;
		
	std::string s = var.Str() ;
	req->PrintF( "Content-type: application/json\r\n\r\n%s\r\n\r\n", s.c_str() ) ;
}

void RootServer::ServeIndex( Request *req, const Resource& res )
{
	req->PrintF( "Content-type: text/html\r\n\r\n" ) ;

	req->PrintF( "<ul>" ) ;
	
	// show the [parent] entry
	if ( res.Path().parent_path() != "/" )
		req->PrintF( "<li class=\"idx_folder menu_idx\"><a href=\"%1%/%2%\">[parent]</a></li>",
			res.UrlPath().parent_path().parent_path().generic_string(),
			m_main_page) ;
	
	if ( fs::is_directory(res.ContentPath().parent_path()) )
	{
		fs::directory_iterator di( res.ContentPath().parent_path() ), end ;
		for ( ; di != end ; ++di )
		{
			Resource sibling(
				(res.UrlPath().parent_path() /
				di->path().filename()).string() ) ;

			static const std::string
				file_class		= "idx_file",
				folder_class	= "idx_folder" ;

			req->PrintF( "<li class=\"%1% menu_idx\"><a href=\"%2%\">%3%</a></li>",
				(fs::is_directory( di->path() ) ? folder_class : file_class),				
				sibling.UrlPath().generic_string(),
				(fs::is_directory( di->path() ) ? sibling.ParentName() : sibling.Name()) ) ;
		}
	}
	req->PrintF( "</ul>\r\n\r\n" ) ;
}

void RootServer::ServeMimeCss( Request *req, const Resource& res )
{
	req->PrintF( "Content-type: text/css\r\n\r\n" ) ;
	req->PrintF( ".haha {\nwidth:100%;\n}\r\n\r\n" ) ;
}

} // end of namespace

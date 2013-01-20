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
#include "log/Log.hh"
#include "parser/FormData.hh"
#include "util/File.hh"

#include <boost/regex.hpp>
#include <boost/bind.hpp>

namespace wb {

RootServer::RootServer( ) :
	m_lib_path	( fs::canonical( cfg::Path("base") / cfg::Path("lib_path") ) ),
	m_data_path	( fs::canonical( cfg::Path("base") / cfg::Path("data_path") ) ),
	m_wb_root	( cfg::Inst()["wb_root"].Str() ),
	m_main_page	( cfg::Inst()["main_page"].Str() )
{
	// GET requests
	Query<Handler>& get = m_srv.insert( std::make_pair( 
		"GET",
		Query<Handler>(&RootServer::NotFound) ) ).first->second ;
	
	get.Add( "",		&RootServer::DefaultPage ) ;
	get.Add( "mime",	&RootServer::ServeMimeCss ) ;
	get.Add( "var",		&RootServer::ServeVar ) ;
	get.Add( "index",	&RootServer::ServeIndex ) ;
	get.Add( "load",	&RootServer::Load ) ;
	get.Add( "lib",		&RootServer::ServeLib ) ;

	// POST requests
	Query<Handler>& post = m_srv.insert( std::make_pair( 
		"POST",
		Query<Handler>(&RootServer::NotFound) ) ).first->second ;
	post.Add( "save",	&RootServer::Save ) ;
	post.Add( "upload",	&RootServer::Upload ) ;
}
	
void RootServer::Work( Request *req, const Resource& res ) 
{
	fs::path	rel		= res.Path() ;
	
	if ( res.UrlPath() != req->SansQueryURI() )
		req->SeeOther( res.UrlPath().string(), true ) ;
	else
	{
		std::string qstr	= req->Query() ;

		Map::const_iterator i = m_srv.find( req->Method() ) ;
		if ( i != m_srv.end() )
		{
			Handler h = i->second.Parse( qstr ) ;
			assert( !h.empty() ) ;
			h( this, req, res ) ;
		}
		else
			NotFound( req ) ;
	}
}

void RootServer::DefaultPage( Request *req, const Resource& res )
{
	if ( res.Type() == "text/html" )
	{
		Log( "serving home page for %1%", res.Path(), log::verbose ) ;
		req->XSendFile( m_lib_path / "index.html" ) ;
	}
	else
		ServeDataFile( req, res ) ;
}

void RootServer::Load( Request *req, const Resource& res )
{
	if ( !ServeDataFile( req, res ) )
		ServeLibFile( req, "newpage.html" ) ;
}

void RootServer::Save( Request *req, const Resource& res )
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
	
void RootServer::Upload( Request *req, const Resource& res )
{
	FormData form( req->In(), req->ContentType() ) ;
	form.Save( res.ContentPath().parent_path() ) ;
}

void RootServer::ServeLib( Request *req, const Resource& res )
{
	static const boost::regex re( "lib=(.+)" ) ;
	boost::smatch m ;
	
	// must make a copy before calling regex_search
	std::string qstr = req->Query() ;
	if ( boost::regex_search( qstr, m, re ) )
	{
		fs::path p( m[1].str() ) ;

		if ( std::find( p.begin(), p.end(), ".." ) != p.end() )
			NotFound( req ) ;

		// only serve file if it is in the root path
		else if ( res.Path().parent_path() == "/" )
		{
			ServeLibFile( req, p ) ;
		}

		// request for lib file using non-root paths will get a redirect
		else
		{
			std::string path = m_wb_root + "main?lib=" + p.string() ;
			req->SeeOther( m_wb_root + "/" + m_main_page + "?lib=" + p.string() ) ;
		}
	}
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

void RootServer::ServeLibFile( Request *req, const fs::path& libfile )
{
	fs::path path = m_lib_path / libfile ;
	Log( "serving lib file: %1% %2%", path, cfg::MimeType(path), log::verbose ) ;
	
	if ( fs::exists(path) )
	{
		req->PrintF( "Content-type: %1%\r\n", cfg::MimeType(path) ) ;
		req->XSendFile( path ) ;
	}
	else
		NotFound( req ) ;
}

void RootServer::ServeVar( Request *req, const Resource& )
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

void RootServer::ServeMimeCss( Request *req, const Resource& )
{
	req->PrintF( "Content-type: text/css\r\n\r\n" ) ;
	req->PrintF( ".haha {\nwidth:100%;\n}\r\n\r\n" ) ;
}

void RootServer::NotFound( Request *req, const Resource& )
{
	req->NotFound("<h1>Not Found!!!</h1>") ;
}

} // end of namespace

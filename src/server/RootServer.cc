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
#include "util/PrintF.hh"

#include <boost/regex.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <set>
#include <sstream>

namespace wb {

RootServer::RootServer( ) :
	m_lib_path	( fs::canonical( cfg::Path("base") / cfg::Path("lib_path") ) ),
	m_data_path	( fs::canonical( cfg::Path("base") / cfg::Path("data_path") ) ),
	m_wb_root	( cfg::Inst()["wb_root"].Str() ),
	m_main_page	( cfg::Inst()["main_page"].Str() ),
	m_mime_css	( GenerateMimeCss(m_lib_path) )
{
	// handlers for GET requests
	Query<Handler>& get = m_srv.insert( std::make_pair( 
		"GET",
		Query<Handler>(&RootServer::NotFound) ) ).first->second ;
	
	get.Add( "",		&RootServer::DefaultPage ) ;
	get.Add( "mime",	&RootServer::ServeMimeCss ) ;
	get.Add( "var",		&RootServer::ServeVar ) ;
	get.Add( "index",	&RootServer::ServeIndex ) ;
	get.Add( "load",	&RootServer::Load ) ;
	get.Add( "lib",		&RootServer::ServeLib ) ;

	// handlers for POST requests
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
		ServeFile( req, m_lib_path / "index.html", cfg::Inst()["cache"]["lib"].Int() ) ;

	else
		ServeFile( req, res.ContentPath(), cfg::Inst()["cache"]["data"].Int() ) ;
}

void RootServer::Load( Request *req, const Resource& res )
{
	if ( !ServeFile( req, res.ContentPath(), cfg::Inst()["cache"]["data"].Int() ) )
		ServeFile( req, m_lib_path / "newpage.html", cfg::Inst()["cache"]["lib"].Int() ) ;
}

void RootServer::Save( Request *req, const Resource& res )
{
	fs::path 	file	= res.ContentPath() ;
	Log( "writing to file %1%", file, log::verbose ) ;
		
	fs::create_directories( file.parent_path() ) ;
	File f( file, 0600 ) ;
		
	char buf[1024] ;
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
			if ( !ServeFile( req, m_lib_path/p, cfg::Inst()["cache"]["lib"].Int() ) )
				NotFound( req ) ;
		}

		// request for lib file using non-root paths will get a redirect
		else
		{
			req->SeeOther( m_wb_root + "/" + m_main_page + "?lib=" + p.string() ) ;
		}
	}
}

bool RootServer::ServeFile( Request *req, const fs::path& path, int cache_age )
{
//	Log( "serving file: %1% %2%", path, cfg::MimeType(path), log::verbose ) ;
	if ( fs::exists(path) )
	{
		PrintF fmt = req->Fmt() ;
		if ( cache_age > 0 )
			fmt( "Cache-Control: max-age=%1%\r\n", cache_age ) ;

		req->XSendFile( path.string() ) ;
		return true ;
	}
	else
		return false ;
}

void RootServer::ServeVar( Request *req, const Resource& )
{
	// path to URLs should be generic strings
	Json var ;
	var.Add( "name", Json( cfg::Inst()["name"] ) ) ;
	var.Add( "wb_root", Json( m_wb_root ) ) ;
	var.Add( "main", Json( m_main_page ) ) ;
	
	PrintF fmt = req->Fmt() ;
	fmt( "Cache-Control: max-age=%1%\r\n", cfg::Inst()["cache"]["lib"].Int() ) ;
	fmt( "Content-type: application/json\r\n\r\n%s\r\n\r\n", var.Str() ) ;
}

void RootServer::ServeIndex( Request *req, const Resource& res )
{
	PrintF fmt = req->Fmt() ;
	
	fmt( "Cache-Control: max-age=%1%\r\n", cfg::Inst()["cache"]["index"].Int() ) ;
	fmt( "Content-type: text/html\r\n\r\n" ) ;
	fmt( "<ul>" ) ;
	
	// show the [parent] entry
	if ( res.Path().parent_path() != "/" )
		fmt( "<li class=\"inode-directory menu_idx\"><a href=\"%1%/%2%\">[parent]</a></li>",
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

			std::string type = sibling.Type() ;
			std::replace( type.begin(), type.end(), '/', '-' ) ;
			std::replace( type.begin(), type.end(), '+', '-' ) ;

			fmt( "<li class=\"%1% menu_idx\"><a href=\"%2%\">%3%</a></li>",
				(fs::is_directory( di->path() ) ? "inode-directory" : type),
				sibling.UrlPath().generic_string(),
				(fs::is_directory( di->path() ) ? sibling.ParentName() : sibling.Name()) ) ;
		}
	}
	fmt( "</ul>\r\n\r\n" ) ;
}

void RootServer::ServeMimeCss( Request *req, const Resource& )
{
	req->Out()->Write( m_mime_css.c_str(), m_mime_css.size() ) ;
}

void RootServer::NotFound( Request *req, const Resource& )
{
	req->NotFound("<h1>Not Found!!!</h1>") ;
}

std::string RootServer::GenerateMimeCss( const fs::path& lib_path )
{
	std::ostringstream ss ;
	ss	<< "Content-type: text/css\r\n\r\n" ;

	const fs::path icon_path = lib_path / "icons" ;
	std::set<std::string> mime_set ;
	mime_set.insert( "inode-directory" ) ;
	mime_set.insert( "application-octet-stream" ) ;
	
	Json::Object mime = cfg::Inst()["mime"].AsObject() ;
	for ( Json::Object::iterator i = mime.begin() ; i != mime.end() ; ++i )
	{
		std::string type = i->second.Str() ;
		std::replace( type.begin(), type.end(), '/', '-' ) ;
		std::replace( type.begin(), type.end(), '+', '-' ) ;
		
		mime_set.insert( type ) ;
	}
	
	for ( std::set<std::string>::iterator i = mime_set.begin() ; i != mime_set.end() ; ++i )
	{
		ss	<< "." << *i << ":hover, ." << *i
			<< "{background-image:url(\"?lib=icons/" ;
		
		if ( fs::exists(icon_path / (*i + ".png")) )
			ss << *i << ".png" ;
		else if ( fs::exists(icon_path / (*i + ".svg")) )
			ss << *i << ".svg" ;
		else
			ss << "application-octet-stream.svg" ;
		
		ss << "\");}\n" ;
	}
	ss << "\r\n\r\n" ;
	
	return ss.str() ;
}

} // end of namespace

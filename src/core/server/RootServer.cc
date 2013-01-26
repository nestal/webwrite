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

#ifdef HAVE_LIBXML2
	#include "xml/HtmlValidator.hh"
#endif

#include <boost/regex.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <set>
#include <sstream>

namespace wb {

RootServer::RootServer( ) :
	m_lib_redir	( cfg::Inst()["lib"]["redir"].Str() ),
	m_data_path	( cfg::Inst()["data"]["path"].Str() ),
	m_wb_root	( cfg::Inst()["wb_root"].Str() ),
	m_main_page	( cfg::Inst()["main_page"].Str() ),
	m_mime_css	( GenerateMimeCss() )
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

int RootServer::ReadOptionalIntConfig( const std::string& base, const std::string& item, int def_value )
{
	Json base_json, item_json;

	return ( cfg::Inst().Get( base, base_json ) && base_json.Get( item, item_json ) )
		? item_json.Int()
		: def_value ;
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
		ServeFile( req, m_lib_redir / "index.html" ) ;

	else
		ServeFile( req, res.ReDirPath() ) ;
}

void RootServer::Load( Request *req, const Resource& res )
{
	// don't cache for dynamic contents
	req->Fmt()( "Cache-Control: max-age=%1%\r\n", 0 ) ;
	
	if ( fs::exists( res.DataPath() ) )
		ServeFile( req, res.ReDirPath() ) ;
	else
		ServeFile( req, m_lib_redir / "newpage.html" ) ;
}

void RootServer::Save( Request *req, const Resource& res )
{
	fs::path 	file	= res.DataPath() ;
	Log( "writing to file %1%", file, log::verbose ) ;
		
	fs::create_directories( file.parent_path() ) ;
	File f( file, 0600 ) ;

	DataStream *out = &f ;

#ifdef HAVE_LIBXML2
	HtmlValidator	html( out, file.filename().string() ) ;
	out = &html ;
#endif

	char buf[1024] ;
	std::size_t c ;
	while ( (c = req->In()->Read(buf, sizeof(buf)) ) > 0 )
		out->Write( buf, c ) ;

#ifdef HAVE_LIBXML2
	html.Finish() ;
#endif

	// ask client to load the new content again
	req->SeeOther( res.UrlPath().generic_string() + "?load" ) ;
}
	
void RootServer::Upload( Request *req, const Resource& res )
{
	FormData form( req->In(), req->ContentType() ) ;
	form.Save( res.DataPath().parent_path() ) ;
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
			ServeFile( req, m_lib_redir/p ) ;

		// request for lib file using non-root paths will get a redirect
		else
		{
			req->SeeOther( m_wb_root + "/" + m_main_page + "?lib=" + p.string() ) ;
		}
	}
}

void RootServer::ServeFile( Request *req, const fs::path& path )
{
	Log( "serving file: %1% %2%", path.generic_string(), cfg::MimeType(path), log::verbose ) ;
	req->XSendFile( path.generic_string() ) ;
}

void RootServer::ServeVar( Request *req, const Resource& )
{
	// path to URLs should be generic strings
	Json var ;
	var.Add( "name", Json( cfg::Inst()["name"] ) ) ;
	var.Add( "wb_root", Json( m_wb_root ) ) ;
	var.Add( "main", Json( m_main_page ) ) ;
	
	PrintF fmt = req->Fmt() ;
	fmt( "Cache-Control: max-age=%1%\r\n", 3600 ) ;
	fmt( "Content-type: application/json\r\n\r\n%s\r\n\r\n", var.Str() ) ;
}

void RootServer::ServeIndex( Request *req, const Resource& res )
{
	PrintF fmt = req->Fmt() ;
	
	fmt( "Cache-Control: max-age=%1%\r\n", 0 ) ;
	fmt( "Content-type: text/html\r\n\r\n" ) ;
	fmt( "<ul>" ) ;
	
	// show the [parent] entry
	if ( res.Path().parent_path() != "/" )
		fmt( "<li class=\"inode-directory menu_idx\"><a href=\"%1%/%2%\">[parent]</a></li>",
			res.UrlPath().parent_path().parent_path().generic_string(),
			m_main_page) ;
	
	if ( fs::is_directory(res.DataPath().parent_path()) )
	{
		fs::directory_iterator di( res.DataPath().parent_path() ), end ;
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

std::string RootServer::GenerateMimeCss( )
{
	std::ostringstream ss ;
	ss	<< "Content-type: text/css\r\n\r\n" ;

	const fs::path icon_path = cfg::Inst()["lib"]["path"].Str() + "/icons" ;
	std::set<std::string> mime_set ;
	mime_set.insert( "inode-directory" ) ;
	mime_set.insert( "application-octet-stream" ) ;
	
	// generate a unique list of configured mime type 
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
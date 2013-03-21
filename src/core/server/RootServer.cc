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
#include "parser/HTMLStreamFilter.hh"
#include "util/Atomic.hh"
#include "util/File.hh"
#include "util/OutputStream.hh"

#include <boost/regex.hpp>
#include <boost/bind.hpp>
#include <boost/timer/timer.hpp>
#include <boost/tokenizer.hpp>

#include <ctime>
#include <set>
#include <sstream>

#include <sys/stat.h>

namespace wb {

RootServer::RootServer( ) :
	m_lib_redir	( Cfg::Inst().lib.redir ),
	m_lib_path	( Cfg::Inst().lib.path ),
	m_meta_redir( Cfg::Inst().meta.redir ),
	m_data_path	( Cfg::Inst().data.path ),
	m_wb_root	( Cfg::Inst().wb_root ),
	m_main_page	( Cfg::Inst().main_page ),
	m_mime_css	( GenerateMimeCss() ),
	m_index_html( IndexHtml() )
{
	// handlers for GET requests
	Query<Handler>& get = m_srv.insert( std::make_pair( 
		"GET",
		Query<Handler>(Handler(&RootServer::NotFound)) ) ).first->second ;
	
	get.Add( "",		Handler(&RootServer::DefaultPage) ) ;
	get.Add( "prop",	Handler(&RootServer::ServeIndexPage) ) ;
	get.Add( "page",	Handler(&RootServer::ServeIndexPage) ) ;
	get.Add( "mime",	Handler(&RootServer::ServeMimeCss) ) ;
	get.Add( "var",		Handler(&RootServer::ServeVar) ) ;
	get.Add( "index",	Handler(&RootServer::ServeIndex) ) ;
	get.Add( "data",	Handler(&RootServer::Load) ) ;
	get.Add( "meta",	Handler(&RootServer::ServeMeta) ) ;
	get.Add( "stats",	Handler(&RootServer::ServeStats) ) ;

	// handlers for POST requests
	Query<Handler>& post = m_srv.insert( std::make_pair( 
		"POST",
		Query<Handler>(Handler(&RootServer::NotFound)) ) ).first->second ;
	post.Add( "data",	Handler(&RootServer::Save) ) ;
	post.Add( "upload",	Handler(&RootServer::Upload) ) ;
}

void RootServer::Work( Request *req, const Resource& res ) 
{
	using boost::timer::cpu_timer;
	using boost::timer::cpu_times;
	using boost::timer::nanosecond_type;
	
	cpu_timer timer;
	fs::path	rel		= res.Path() ;
	
	if ( res.CheckRedir(req->SansQueryURI()) )
	{
		Log( "redirecting %1% to %2%", req->URI(), res.UrlPath() ) ;
		req->SeeOther( res.UrlPath().string(), true ) ;
	}
	else
	{
		std::string qstr	= req->Query() ;

		Map::iterator i = m_srv.find( req->Method() ) ;
		if ( i != m_srv.end() )
		{
			Handler& h = i->second.Parse( qstr ) ;
			
			// do real work
			assert( !h.func.empty() ) ;
			h.func( this, req, res ) ;

			// calculate time elapsed for processing
			h.elapse += static_cast<long>(timer.elapsed().wall) ;
			
			// bump stats
			h.count++ ;
		}
		else
			NotFound( req ) ;
	}
}

void RootServer::ServeIndexPage( Request *req, const Resource& )
{
	req->CacheControl(0) ;
	req->Send( m_index_html ) ;
}

void RootServer::DefaultPage( Request *req, const Resource& res )
{
	if ( IsLibFile(res) )
		ServeLib( req, res ) ;

	else if ( res.Type() == "text/html" )
		ServeIndexPage( req, res ) ;

	else
		ServeFile( req, res.ReDirPath() ) ;
}

void RootServer::Load( Request *req, const Resource& res )
{
	// don't cache for dynamic contents
	req->CacheControl(0) ;
	
	if ( fs::exists( res.DataPath() ) )
		ServeFile( req, res.ReDirPath() ) ;
	
	else if ( IsLibFile(res) )
		ServeLib( req, res ) ;

	else
	{
		std::string percent20name, space_name, tmp = res.DataPath().string() ;
		for ( std::string::iterator i = tmp.begin(); i != tmp.end() ; i++ )
		{
			if ( *i == '_' )
			{
				percent20name.insert( percent20name.size(), "%20" ) ;
				space_name.push_back( ' ' ) ;
			}
			else
			{
				percent20name.push_back( *i ) ;
				space_name.push_back( *i ) ;
			}
		}
		fs::path percent20path( percent20name ), space_path( space_name ) ;

		if ( fs::exists( percent20path ) )
		{
			fs::rename( percent20path, res.DataPath() ) ;
			ServeFile( req, res.ReDirPath() ) ;
		}
		else if ( fs::exists( space_path ) )
		{
			fs::rename( space_path, res.DataPath() ) ;
			ServeFile( req, res.ReDirPath() ) ;
		}
		else
			ServeFile( req, m_lib_redir / "newpage.html" ) ;
	}
}

void RootServer::FilterHTML( DataStream *html, const Resource& res )
{
	fs::path file = res.DataPath() ;
	
	HTMLStreamFilter filter;
	
	fs::create_directories( file.parent_path() ) ;
	{
		File dest( file, 0600 ) ;
		filter.Parse( html, &dest ) ;
	}
	
	// save metadata after closing the file	
	if ( fs::file_size(file) > 0 )
		res.SaveMeta() ;
}

void RootServer::Save( Request *req, const Resource& res )
{
	// only save when the file is NOT a lib file
	if ( !IsLibFile(res) )
	{
		fs::path file = res.DataPath() ;
		Log( "writing to file %1%", file, log::verbose ) ;

		// first move the original file to attic without any modification
		res.MoveToAttic() ;
	
		// read input data, do filtering and save the file
		FilterHTML( req->In(), res ) ;

		// if the new file is empty, that means the user wants to delete the file
		boost::uintmax_t size = fs::file_size(file) ;
		if ( size == 0 )
		{
			Log( "Removing file %1%", file ) ;
			res.Remove() ;
		}

		// only save meta if the file is not removed
		else
			res.SaveMeta() ;
	}

	// ask client to load the new content again. the client may not reload the page with the
	// new content (because it has it anyway), but this will update the client's cache.
	req->SeeOther( res.UrlPath().generic_string() + "?data" ) ;
}
	
void RootServer::Upload( Request *req, const Resource& res )
{
	FormData form( req->In(), req->ContentType() ) ;
	form.Save( res.DataPath().parent_path(), OnFileUploaded ) ;
	
	req->Success() ;
}

void RootServer::OnFileUploaded(
		const fs::path&		path,
		const std::string&	filename,
		File&				file,
		const std::string&	mime )
{
	// TODO: construct a Resource and setup the right metadata
}

bool RootServer::IsLibFile( const Resource& res ) const
{
	fs::path p = m_lib_path/res.Path() ;
	return fs::exists(p) && !fs::is_directory(p) ;
}

void RootServer::ServeLib( Request *req, const Resource& res )
{
	ServeFile( req, m_lib_redir/res.Path() ) ;
}

void RootServer::ServeFile( Request *req, const fs::path& path )
{
	req->XSendFile( path.generic_string() ) ;
}

void RootServer::ServeVar( Request *req, const Resource& )
{
	// path to URLs should be generic strings
	Json var ;
	var.Add( "name", Json( Cfg::Inst().name ) ) ;
	var.Add( "wb_root", Json( m_wb_root ) ) ;
	var.Add( "main", Json( m_main_page ) ) ;
	
	req->Send( var ) ;
}

void RootServer::ServeIndex( Request *req, const Resource& res )
{
	req->CacheControl(0) ;
	
	OutputStream os( req->Out() ) ;
	os << 
		"Content-type: text/html\r\n\r\n"
		"<ul>" ;
	
	// show the [parent] entry
	if ( res.Path().parent_path() != "/" )
		os	<< "<li class=\"inode-directory menu_idx\"><a href=\""
			<< res.UrlPath().parent_path().parent_path().generic_string()
			<< '/' << m_main_page
			<< "\">[parent]</a></li>" ;
	
	if ( fs::is_directory(res.DataPath().parent_path()) )
	{
		fs::directory_iterator di( res.DataPath().parent_path() ), end ;
		for ( ; di != end ; ++di )
		{
			Resource sibling(
				(res.UrlPath().parent_path() /
				di->path().filename()).string() ) ;

			std::string type = CssMimeType(sibling.Type()) ;

			os	<< "<li class=\""
				<< (fs::is_directory( di->path() ) ? "inode-directory" : type)
				<< " menu_idx\"><a href=\""
				<< sibling.UrlPath().generic_string()
				<< ((type == "text-html" || fs::is_directory(di->path())) ? "" : "?prop")
				<< "\">"
				<< (fs::is_directory( di->path() ) ? sibling.ParentName() : sibling.Name())
				<< "</a></li>" ;
		}
	}
	os << "</ul>\r\n\r\n" ;
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

	const fs::path icon_path = Cfg::Inst().lib.path / "icons" ;
	std::set<std::string> mime_set ;
	mime_set.insert( "inode-directory" ) ;
	mime_set.insert( "application-octet-stream" ) ;
	
	// generate a unique list of configured mime type 
	for ( Cfg::MimeMap::const_iterator i = Cfg::Inst().mime.begin() ;
		i != Cfg::Inst().mime.end() ; ++i )
		mime_set.insert( CssMimeType(i->second)) ;
	
	for ( std::set<std::string>::iterator i = mime_set.begin() ; i != mime_set.end() ; ++i )
	{
		ss	<< "." << *i << ":hover, ." << *i
			<< "{background-image:url(\"" << Cfg::Inst().wb_root << "/icons/" ;
		
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

std::string RootServer::IndexHtml( )
{
	File idx(Cfg::Inst().lib.path / "index.html") ;

	std::string html ;
	char temp[80] ;
	
	std::size_t count = 0 ;
	while ( (count = idx.Read( &temp[0], sizeof(temp) ) ) > 0 )
		html.insert( html.end(), temp, temp + count ) ;

	boost::format fmt(html) ;
	return (fmt % Cfg::Inst().dep_uri % Cfg::Inst().wb_root).str() ;
}

std::string RootServer::CssMimeType( const std::string& mime )
{
	std::string type( mime ) ;
	std::replace( type.begin(), type.end(), '/', '-' ) ;
	std::replace( type.begin(), type.end(), '+', '-' ) ;
	std::replace( type.begin(), type.end(), '.', '-' ) ;
	return type ;
}

void RootServer::ServeMeta( Request *req, const Resource& res )
{
	fs::path meta = res.MetaPath();
	if ( !IsLibFile(res) && !fs::exists(meta) )
		res.SaveMeta() ;
	
	req->CacheControl(0) ;
	req->XSendFile( (m_meta_redir / res.Path()).string() ) ;
}

void RootServer::ServeStats( Request *req, const Resource& res )
{
	Json result ;
	for ( Map::const_iterator s = m_srv.begin(); s != m_srv.end() ; ++s )
	{
		const Query<Handler>& q = s->second ;
	
		Json srv ;
		for ( Query<Handler>::iterator i = q.begin() ; i != q.end() ; ++i )
		{
			// avoid volatile-ness
			boost::uint64_t count  = i->second.count ;
			double elapse = i->second.elapse / 1000.0 ;
		
			Json j ;
			j.Add( "count", Json( count ) ) ;
			j.Add( "elapse", Json( elapse ) ) ;
			j.Add( "average", Json( count == 0 ? 0 : elapse / count ) ) ;
			srv.Add( i->first, j ) ;
		}
		result.Add( s->first, srv ) ;
	}
	
	req->CacheControl(0) ;
	req->Send( result ) ;
}

} // end of namespace

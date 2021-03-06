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

#include "FCGIRequest.hh"

#include "util/DataStream.hh"
#include "util/Exception.hh"
#include "util/File.hh"
#include "parser/Json.hh"

#include "log/Log.hh"

#include <cstdio>

namespace wb {

const std::size_t buf_size = 8 * 1024 ;

class FCGIRequest::SrcWrapper : public Source
{
public :
	explicit SrcWrapper( FCGX_Stream *str ) :
		m_str( str )
	{
	}
	
	std::streamsize read(char *data, std::streamsize size) ;

private :
	FCGX_Stream	*m_str ;
} ;

class FCGIRequest::SinkWrapper : public Sink
{
public :
	explicit SinkWrapper( FCGX_Stream *str ) :
		m_str( str )
	{
	}
	
	std::streamsize write(const char *data, std::streamsize size) ;

private :
	FCGX_Stream	*m_str ;
} ;

FCGIRequest::FCGIRequest( FCGX_Request *req ) :
	m_req( req ),
	m_in( new SrcWrapper( m_req->in ) ),
	m_out( new SinkWrapper( m_req->out ) )
{
}

FCGIRequest::~FCGIRequest()
{
}

Source* FCGIRequest::In()
{
	return m_in.get() ;
}

Sink* FCGIRequest::Out()
{
	return m_out.get() ;
}

void FCGIRequest::CacheControl( std::size_t max_age )
{
	FCGX_FPrintF( m_req->out, "Cache-Control: max-age=0\r\n", max_age ) ;
}

void FCGIRequest::Send( const Json& json )
{
	::FCGX_FPrintF( m_req->out, "Content-type: application/json\r\n\r\n" ) ;
	json.Write( m_out.get() ) ;
	::FCGX_FPrintF( m_req->out, "\r\n\r\n" ) ;
}

void FCGIRequest::Send( const std::string& context )
{
	::FCGX_FPrintF( m_req->out, "Content-type: text/html\r\n\r\n" ) ;
	::FCGX_PutStr( context.c_str(), context.size(), m_req->out ) ;
	::FCGX_FPrintF( m_req->out, "\r\n\r\n" ) ;
}

/// Tell the web server to send this file
void FCGIRequest::XSendFile( const std::string& file )
{
	FCGX_FPrintF( m_req->out, "X-Accel-Redirect: %s\r\n\r\n", file.c_str() ) ;
}

std::streamsize FCGIRequest::SrcWrapper::read( char *data, std::streamsize size )
{
	return ::FCGX_GetStr( data, size, m_str ) ;
}

std::streamsize FCGIRequest::SinkWrapper::write( const char *data, std::streamsize size )
{
	return ::FCGX_PutStr( data, size, m_str ) ;
}

std::string FCGIRequest::URI() const
{
	return ::FCGX_GetParam( "REQUEST_URI", m_req->envp ) ;
}

void FCGIRequest::PrintEnv() const
{
	for ( int i = 0 ; m_req->envp[i] != 0 ; ++i )
		Log( "env: %1%", m_req->envp[i], log::debug );
}

std::string FCGIRequest::Method() const
{
	return ::FCGX_GetParam( "REQUEST_METHOD", m_req->envp ) ;
}

std::string FCGIRequest::Referer() const
{
	return ::FCGX_GetParam( "HTTP_REFERER", m_req->envp ) ;
}

std::string FCGIRequest::Query() const
{
	return ::FCGX_GetParam( "QUERY_STRING", m_req->envp ) ;
}

std::string FCGIRequest::ContentType() const
{
	return ::FCGX_GetParam( "CONTENT_TYPE", m_req->envp ) ;
}

void FCGIRequest::SeeOther( const std::string& location, bool query )
{
	FCGX_FPrintF(
		m_req->out,
		"Status: 303 See Other\r\nLocation: %s\r\n\r\n",
		(query && !Query().empty()) ?
			(location + '?' + Query() ).c_str() : 
			location.c_str() ) ;
}

void FCGIRequest::NotFound( const std::string& message )
{
	FCGX_FPrintF( m_req->out,
		"Status: 404 Not Found\r\n\r\n"
		"<html><body>%s</body></html>\r\n\r\n",
		message.c_str() ) ;
}

void FCGIRequest::Success()
{
	FCGX_FPrintF( m_req->out, "Status: 200 OK\r\n\r\n" ) ;
}

std::string FCGIRequest::SansQueryURI() const
{
	std::string uri		= URI() ;
	std::string	query	= Query() ;
	std::string result	= uri.substr( 0, uri.size() - query.size() ) ;
	
	// remove the '?' if any
	if ( !query.empty() && !result.empty() && result[result.size()-1] == '?' )
		result.erase( result.size()-1 ) ;
	
	return result ;
}

} // end of namespace

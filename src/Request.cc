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

#include "Request.hh"

#include "util/DataStream.hh"
#include "util/Exception.hh"
#include "util/File.hh"

#include <iostream>

namespace wb {

const std::size_t buf_size = 8 * 1024 ;

class Request::StreamWrapper : public DataStream
{
public :
	explicit StreamWrapper( FCGX_Stream *str ) :
		m_str( str )
	{
	}
	
	std::size_t Read( char *data, std::size_t size ) ;
	std::string ReadLine( std::size_t max ) ;
	std::size_t Write( const char *data, std::size_t size ) ;
	bool GetChar( char& ch ) ;

private :
	FCGX_Stream	*m_str ;
} ;

Request::Request( FCGX_Request *req ) :
	m_req( req ),
	m_in( new StreamWrapper( m_req->in ) ),
	m_out( new StreamWrapper( m_req->out ) )
{
}

Request::~Request()
{
}

DataStream* Request::In()
{
	return m_in.get() ;
}

DataStream* Request::Out()
{
	return m_out.get() ;
}

void Request::XSendFile( const fs::path& file )
{
	FCGX_FPrintF( m_req->out, "X-Sendfile: %s\r\n\r\n", file.string().c_str() ) ;
}

std::size_t Request::StreamWrapper::Read( char *data, std::size_t size )
{
	return ::FCGX_GetStr( data, size, m_str ) ;
}

std::size_t Request::StreamWrapper::Write( const char *data, std::size_t size )
{
	return ::FCGX_PutStr( data, size, m_str ) ;
}

bool Request::StreamWrapper::GetChar( char& result )
{
	int ch = ::FCGX_GetChar( m_str ) ;
	if ( ch != EOF )
		result = static_cast<char>(ch) ;
	return ch != EOF ;
}

std::string Request::URI() const
{
	return ::FCGX_GetParam( "REQUEST_URI", m_req->envp ) ;
}

void Request::PrintEnv() const
{
	for ( int i = 0 ; m_req->envp[i] != 0 ; ++i )
		std::cout << "env: " << m_req->envp[i] << std::endl ;
}

std::string Request::Method() const
{
	return ::FCGX_GetParam( "REQUEST_METHOD", m_req->envp ) ;
}

std::string Request::Referer() const
{
	return ::FCGX_GetParam( "HTTP_REFERER", m_req->envp ) ;
}

std::string Request::Query() const
{
	return ::FCGX_GetParam( "QUERY_STRING", m_req->envp ) ;
}

std::string Request::ContentType() const
{
	return ::FCGX_GetParam( "CONTENT_TYPE", m_req->envp ) ;
}

std::size_t Request::PrintF( const char *fmt )
{
	return FCGX_FPrintF( m_req->out, fmt ) ;
}

void Request::SeeOther( const std::string& location )
{
	FCGX_FPrintF(
		m_req->out,
		"303 See Other\r\nLocation: %s\r\n\r\n",
		location.c_str() ) ;
}

std::string Request::SansQueryURI() const
{
	std::string uri		= URI() ;
	std::string	query	= Query() ;
	std::string result	= uri.substr( 0, uri.size() - query.size() ) ;
	
	// remove the '?' if any
	if ( !query.empty() && !result.empty() && result[result.size()-1] == '?' )
		result.erase( result.size()-1 ) ;
	
	return result ;
}

std::string Request::StreamWrapper::ReadLine( std::size_t max )
{
	std::string result(max, '\0') ;
	if ( FCGX_GetLine( &result[0], result.size(), m_str ) != 0 )
	{
		result.resize( std::strlen( result.c_str() ) ) ;
		return result ;
	}
	else
		return "" ;
}

} // end of namespace

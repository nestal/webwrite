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

#include "util/Exception.hh"
#include "util/File.hh"

namespace wb {

const std::size_t buf_size = 8 * 1024 ;

Request::Request( FCGX_Request *req ) :
m_req( req )
{
}

std::size_t Request::SendFile( const fs::path& file, const std::string& mime )
{
	// content type header
	FCGX_FPrintF( m_req->out,
		"Content-type: %s\r\n"
		"\r\n",
		mime.c_str() ) ;

	File f( file ) ;

	char buf[buf_size] ;
	std::size_t count, total = 0 ;
	while ( (count = f.Read( buf, sizeof(buf) )) > 0 )
	{
		int r = ::FCGX_PutStr( buf, count, m_req->out ) ;
		if ( r == -1 )
			throw Exception() ;
		
		total += r ;
	}
	
	return total ;
}

std::size_t Request::Recv( char *data, std::size_t size )
{
	return ::FCGX_GetStr( data, size, m_req->in ) ;
}

std::size_t Request::Send( const char *data, std::size_t size )
{
	return ::FCGX_PutStr( data, size, m_req->out ) ;
}

std::string Request::URI() const
{
	return ::FCGX_GetParam( "REQUEST_URI", m_req->envp ) ;
}

} // end of namespace

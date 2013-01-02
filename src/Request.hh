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

#pragma once

#include "util/FileSystem.hh"

#include "fcgiapp.h"

#include <cstddef>
#include <memory>

namespace wb {

class Config ;
class DataStream ;

/*!	\brief HTTP request object

	This class encapsulates all HTTP related stuff, including formatting the message
	body; returning status code; and other HTTP headers.
*/
class Request
{
public :
	explicit Request( FCGX_Request *req ) ;

	bool IsPost() const ;
	
	// request environment
	std::string URI() const ;
	std::string SansQueryURI() const ;
	std::string Method() const ;
	std::string Referer() const ;
	std::string Query() const ;
	
	DataStream* In() ;
	DataStream* Out() ;

	// templated version of printf
	std::size_t PrintF( const char *fmt ) ;
	
	template <typename P1>
	std::size_t PrintF( const char *fmt, const P1& p1 ) ;

	template <typename P1, typename P2>
	std::size_t PrintF( const char *fmt, const P1& p1, const P2& p2 ) ;

	template <typename P1, typename P2, typename P3>
	std::size_t PrintF( const char *fmt, const P1& p1, const P2& p2, const P3& p3 ) ;

	// tell the web server to send this file
	void XSendFile( const fs::path& file ) ;
	
	// HTTP status return
	void SeeOther( const std::string& location ) ;
	
	// for debugging
	void PrintEnv() const ;
	
private :
	FCGX_Request	*m_req ;
	
	class StreamWrapper ;
	std::auto_ptr<StreamWrapper> m_in, m_out ;
} ;

template <typename P1>
std::size_t Request::PrintF( const char *fmt, const P1& p1 )
{
	return FCGX_FPrintF( m_req->out, fmt, p1 ) ;

}

template <typename P1, typename P2>
std::size_t Request::PrintF( const char *fmt, const P1& p1, const P2& p2 )
{
	return FCGX_FPrintF( m_req->out, fmt, p1, p2 ) ;

}

template <typename P1, typename P2, typename P3>
std::size_t Request::PrintF( const char *fmt, const P1& p1, const P2& p2, const P3& p3 )
{
	return FCGX_FPrintF( m_req->out, fmt, p1, p2, p3 ) ;

}

} // end of namespace

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

#include "server/Request.hh"

#include <fcgiapp.h>

#include <cstddef>
#include <memory>

namespace wb {

class Json ;
class DataStream ;
class PrintF ;

/*!	\brief HTTP request object

	This class encapsulates all HTTP related stuff, including formatting the message
	body; returning status code; and other HTTP headers.
*/
class FCGIRequest : public Request
{
public :
	explicit FCGIRequest( FCGX_Request *req ) ;
	~FCGIRequest() ;

	// request environment
	std::string URI() const ;
	std::string SansQueryURI() const ;
	std::string Method() const ;
	std::string Referer() const ;
	std::string Query() const ;
	std::string ContentType() const ;
	
	DataStream* In() ;
	DataStream* Out() ;

	// helps
	void CacheControl( std::size_t max_age ) ;
	void Send( const Json& json ) ;
	void XSendFile( const std::string& file ) ;
	
	// HTTP status return
	void SeeOther( const std::string& location, bool query = false ) ;
	void NotFound( const std::string& message ) ;
	void Success() ;
	
	// for debugging
	void PrintEnv() const ;
	
private :
	FCGX_Request	*m_req ;
	
	class StreamWrapper ;
	std::auto_ptr<StreamWrapper> m_in, m_out ;
} ;

} // end of namespace

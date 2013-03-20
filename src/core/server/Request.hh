/*
	webwrite: an GPL wiki-like website with in-place editing
	Copyright (C) 2013 Wan Wai Ho

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	MA  02110-1301, USA.
*/

#pragma once

#include <string>

namespace wb {

class DataStream ;
class OutputStream ;
class Json ;

class Request
{
protected :
	virtual ~Request() {} ;
	
public :
	// request environment
	virtual std::string URI() const = 0 ;
	virtual std::string SansQueryURI() const = 0 ;
	virtual std::string Method() const = 0 ;
	virtual std::string Referer() const = 0 ;
	virtual std::string Query() const = 0 ;
	virtual std::string ContentType() const = 0 ;

	// input and output
	virtual DataStream* In() = 0 ;
	virtual DataStream* Out() = 0 ;

	// helpers
	virtual void CacheControl( std::size_t max_age ) = 0 ;
	virtual void Send( const Json& json ) = 0 ;
	virtual void Send( const std::string& context ) = 0 ;
	virtual void XSendFile( const std::string& file ) = 0 ;

	// HTTP status return
	virtual void SeeOther( const std::string& location, bool query = false ) = 0 ;
	virtual void NotFound( const std::string& message ) = 0 ;
	virtual void Success() = 0 ;
	
	// for debugging
	virtual void PrintEnv() const = 0 ;
} ;

} // end of namespace

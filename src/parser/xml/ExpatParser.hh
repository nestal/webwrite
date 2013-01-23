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

#include "util/DataStream.hh"
#include "util/Exception.hh"
#include <memory>

namespace wb {

class XmlHandler ;

class ExpatParser : public DataStream
{
public :
	struct Error : public Exception {};

	ExpatParser( XmlHandler *out ) ;
	~ExpatParser() ;
	
	std::size_t Read( char *data, std::size_t size ) ;
	std::size_t Write( const char *data, std::size_t size ) ;

private :
	// expat callbacks
	static void StartElement( void *vpthis, const char *element, const char **attr ) ;
	static void EndElement( void *vpthis, const char *element ) ;

private :
	struct Expat ;
	std::auto_ptr<Expat>	m_xpt;
	XmlHandler				*m_out ;
} ;

} // end of namespace


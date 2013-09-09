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

#include "Doc.hh"
#include "Error.hh"

#include "debug/Assert.hh"
#include "util/DataStream.hh"

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <iostream>
#include <cstdio>

namespace xml {

Doc::Doc( const std::string& fname ) :
	Node( reinterpret_cast<xmlNodePtr>(::xmlParseFile( fname.c_str( ) ) ) )
{
	Init() ;
}

void Doc::Init()
{
	if ( Self() == 0 )
		THROW_LAST_XML_EXCEPTION() ;
	
	Self()->_private = this ;
	WB_ASSERT( Check() ) ;
}

Doc::Doc( wb::Source *src ) :
	Node( reinterpret_cast<xmlNodePtr>(::xmlReadIO(
		Doc::ReadCallback,
		Doc::CloseCallback,
		src,
		0,
		0,
		XML_PARSE_RECOVER   | XML_PARSE_PEDANTIC  |
		XML_PARSE_NONET     | XML_PARSE_COMPACT ) )
	)
{
	Init() ;
}

Doc::Doc( const Doc& rhs ) :
	Node( reinterpret_cast<xmlNodePtr>(::xmlCopyDoc(rhs.Self(), 1)) )
{
	Init() ;
}

Doc::Doc( _xmlDoc *self ) :
	Node( reinterpret_cast<xmlNodePtr>(self) )
{
	Init() ;
}

Doc::~Doc()
{
	WB_ASSERT( Check() ) ;
	::xmlFreeDoc( Self() ) ;
}

_xmlDoc* Doc::Self() const
{
	return reinterpret_cast<xmlDocPtr>(Node::Self()) ;
}

int Doc::ReadCallback( void *pvsrc, char *buffer, int len )
{
	wb::Source *src = reinterpret_cast<wb::Source*>(pvsrc) ;
	WB_ASSERT( src != 0 ) ;
	
	return src->read(buffer, len) ;
}

int Doc::CloseCallback( void *pvsrc )
{
	return 0 ;
}

bool Doc::Check() const
{
	return Self()->_private == this ;
}

void Doc::Save( const std::string fname ) const
{
	std::FILE *out = std::fopen( fname.c_str(), "w" ) ;
	if ( out != 0 )
	{
		::xmlDocDump( out, Self() ) ;
		std::fclose(out) ;
	}
}

} // end of namespace

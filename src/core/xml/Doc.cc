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

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <cassert>
#include <iostream>

namespace xml {

Doc::Doc( const std::string& fname ) :
	Node( reinterpret_cast<xmlNodePtr>(::xmlParseFile( fname.c_str( ) ) ) )
{
	Self()->_private = this ;
}

Doc::Doc( const Doc& rhs ) :
	Node( reinterpret_cast<xmlNodePtr>(::xmlCopyDoc( Self(), 1 ) ) )
{
}

Doc::~Doc()
{
	::xmlFreeDoc( Self() ) ;
}

_xmlDoc* Doc::Self() const
{
	return reinterpret_cast<xmlDocPtr>(Node::Self()) ;
}

} // end of namespace

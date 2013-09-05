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

#include <iostream>

namespace xml {

Node::Node() :
	m_node(0)
{
}

Node::Node( _xmlNode *node ) :
	m_node( node )
{
}

xmlNodePtr Node::Get()
{
	return m_node ;
}

Node Node::Children() const
{
	return Node(m_node->children) ;
}

Node Node::operator[]( const std::string& sel ) const
{
	for ( xmlNodePtr n = m_node->children ; n != 0 ; n = n->next )
	{
		if ( sel == reinterpret_cast<const char*>(n->name) )
			return Node(n) ;
	}
	return Node() ;
}

OwnedNode::OwnedNode( _xmlNode *node ) :
	Node( node )
{
}

OwnedNode::~OwnedNode()
{
	::xmlFreeNode( Get() ) ;
}

Doc::Doc( const std::string& fname ) :
	Node( reinterpret_cast<xmlNodePtr>(::xmlParseFile( fname.c_str( ) ) ) )
{
}

Doc::~Doc()
{
	::xmlFreeDoc( Me() ) ;
}

_xmlDoc* Doc::Me()
{
	return reinterpret_cast<xmlDocPtr>(Get()) ;
}

} // end of namespace

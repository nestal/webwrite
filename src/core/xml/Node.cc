/*
	grive: an GPL program to sync a local directory with Google Drive
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

#include "Node.hh"

#include <libxml/tree.h>

namespace xml {

Node::Node()
{
}

Node::Node( xmlNodePtr node ) : CNode( node )
{
}

Doc* Node::ParentDoc()
{
	return CNode::FromDoc(Self()->doc) ;
}

Node Node::Prev()
{
	return Node(Self()->prev) ;
}

Node Node::Next()
{
	return Node(Self()->next) ;
}

Node Node::Parent()
{
	return Node(Self()->parent) ;
}

Node Node::Children()
{
	return Node(Self()->children) ;
}

Node Node::operator[]( const std::string& sel )
{
	return Children().FindSiblings( sel ) ;
}

Node Node::FindSiblings( const std::string& name )
{
	for ( xmlNodePtr n = Self() ; n != 0 ; n = n->next )
	{
		if ( name == reinterpret_cast<const char*>(n->name) )
			return Node(n) ;
	}
	return Node() ;
}

} // end of namespace

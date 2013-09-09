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

#include "CNode.hh"

#include "Doc.hh"
#include "debug/Assert.hh"

#include <libxml/tree.h>

#include <cassert>

namespace xml {

CNode::CNode() :
	m_node(0)
{
}

CNode::CNode( _xmlNode *node ) :
	m_node( node )
{
}

CNode::~CNode()
{
}

xmlNodePtr CNode::Self() const
{
	return m_node ;
}

CNode CNode::Children() const
{
	return CNode(m_node->children) ;
}

CNode CNode::operator[]( const std::string& sel ) const
{
	return Children().FindSiblings( sel ) ;
}

CNode CNode::FindSiblings( const std::string& name ) const
{
	for ( xmlNodePtr n = m_node ; n != 0 ; n = n->next )
	{
		if ( name == reinterpret_cast<const char*>(n->name) )
			return CNode(n) ;
	}
	return CNode() ;
}

const Doc* CNode::ParentDoc() const
{
	return FromDoc(m_node->doc) ;
}

CNode CNode::Prev() const
{
	return CNode(m_node->prev) ;
}

CNode CNode::Next() const
{
	return CNode(m_node->next) ;
}

CNode CNode::Parent() const
{
	return CNode(m_node->parent) ;
}

std::string CNode::Name() const
{
	return reinterpret_cast<const char*>(m_node->name) ;
}

Doc* CNode::FromDoc( _xmlDoc *doc )
{
	WB_ASSERT( doc != 0 ) ;
	return reinterpret_cast<Doc*>( doc->_private ) ;
}

} // end of namespace

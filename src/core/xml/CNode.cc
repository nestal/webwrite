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

#include <libxml/tree.h>

namespace xml {

CNode::CNode() :
	m_node(0)
{
}

CNode::CNode( _xmlNode *node ) :
	m_node( node )
{
}

xmlNodePtr CNode::Get() const
{
	return m_node ;
}

CNode CNode::Children() const
{
	return CNode(m_node->children) ;
}

CNode CNode::operator[]( const std::string& sel ) const
{
	return CNode(m_node->children).FindSiblings( sel ) ;
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

} // end of namespace

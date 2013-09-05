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

//! XML lib forward declaration
struct _xmlDoc ;
struct _xmlNode ;

namespace xml {

class Node
{
protected :
	explicit Node( _xmlNode *node ) ;

public :
	Node() ;

	Node Children() const ;

	Node operator[]( const std::string& sel ) const ;

protected :
public :
	_xmlNode*	Get() ;

private :
	_xmlNode	*m_node ;
} ;

class OwnedNode : public Node
{
protected :
	explicit OwnedNode( _xmlNode *node ) ;

public :
	OwnedNode() ;
	OwnedNode( const OwnedNode& rhs ) ;
	~OwnedNode() ;
} ;

class Doc : public Node
{
public :
	explicit Doc( const std::string& fname ) ;
	~Doc() ;

protected :
	_xmlDoc* Me() ;
} ;

} // end of namespace


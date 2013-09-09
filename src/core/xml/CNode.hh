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

#pragma once

#include <string>

//! XML lib forward declaration
struct _xmlDoc ;
struct _xmlNode ;

namespace xml {

class Doc ;

class CNode
{
protected :
	explicit CNode( _xmlNode *node ) ;

public :
	CNode() ;
	virtual ~CNode() ;

	// tree access
	CNode Prev() const ;
	CNode Next() const ;
	CNode Parent() const ;
	CNode Children() const ;
	const Doc* ParentDoc() const ;

	// attributes
	std::string Name() const ;

	// operations
	CNode operator[]( const std::string& sel ) const ;
	CNode FindSiblings( const std::string& name ) const ;

protected :
	static Doc* FromDoc( _xmlDoc *doc ) ;
	_xmlNode*	Self() const ;

private :
	_xmlNode	*m_node ;
} ;

} // end of namespace

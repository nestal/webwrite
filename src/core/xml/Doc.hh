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

#include "Node.hh"

namespace wb
{
	class Source ;
	class Sink ;
}

namespace xml {

class Doc : public Node
{
public :
	explicit Doc( const std::string& fname ) ;
	explicit Doc( wb::Source *src ) ;
	Doc( const Doc& rhs ) ;
	~Doc() ;
	
	void Save( const std::string fname ) const ;
	void Save( wb::Sink *sink ) const ;

protected :
	explicit Doc( _xmlDoc *self ) ;

    // callback for read file
    static int ReadCallback( void *pvsrc, char *buffer, int len ) ;
    static int CloseCallback( void *ctx ) ;
    static int WriteCallback( void *pvsink, const char *buffer, int len) ;
    
private :
    void Init() ;

protected :
	_xmlDoc* Self() const ;
	virtual bool Check() const ;
} ;

} // end of namespace


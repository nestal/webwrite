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

#include <map>

namespace wb {

template <typename T>
class Query
{
public :
	explicit Query( const T& def = T() ) : m_default( def )
	{
	}
	
	void Add( const std::string& param, const T& cb )
	{
		m_cb[param] = cb ;
	}
	
	T Parse( const std::string& qstr ) const
	{
		std::string name = qstr.substr( 0, qstr.find_first_of( "&#=" ) ) ;
		
		typename Map::const_iterator i = m_cb.find( name ) ;
		return i != m_cb.end() ? i->second : m_default ;
	}

private :
	typedef std::map<std::string, T> Map ;
	Map	m_cb ;
	T	m_default ;
} ;

} // end of namespace


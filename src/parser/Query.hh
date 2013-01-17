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

#include <boost/function.hpp>
#include <map>

namespace wb {

class Query
{
public :
	Query() ;
	
	typedef boost::function<void(const std::string&)> Callback ;
	
	void Add( const std::string& param, const Callback& cb ) ;
	
	bool Parse( const std::string& qstr ) const ;

private :
	std::map<std::string, Callback>	m_cb ;
} ;

} // end of namespace


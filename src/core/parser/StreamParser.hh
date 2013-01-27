/*
	webwrite: an GPL wiki-like website with in-place editing
	Copyright (C) 2012  Wan Wai Ho

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#pragma once

#include <string>

namespace wb {

class DataStream ;

class StreamParser
{
public :
	explicit StreamParser( DataStream *in ) ;

	std::size_t ReadUntil( const std::string& target, DataStream *out ) ;
	std::size_t ReadUntil( char target, DataStream *out ) ;
	std::size_t ReadUntilFirstOf( const std::string& first_of, DataStream *out ) ;
	std::size_t Consume( std::size_t count, DataStream *out ) ;
	
	bool Refill() ;
	std::size_t Size() const ;
	std::size_t Capacity() const ;

private :
	template <typename Find>
	std::size_t GenericReadUntil( Find find, DataStream *out ) ;

private :
	DataStream	*m_in ;
	char		m_cache[80] ;
	char		*m_end ;
} ;

} // end of namespace

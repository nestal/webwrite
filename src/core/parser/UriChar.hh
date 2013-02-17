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

#include <bitset>
#include <cstdlib>
#include <map>
#include <string>
#include <limits>

namespace wb {

struct Marked
{
	static std::bitset<256> Map()
	{
		// according to RFC2396, these characters are allowed and no need to
		// be escaped
		std::bitset<256> result ;
		result['-']  = true ;
		result['_']  = true ;
		result['.']  = true ;
		result['!']  = true ;
		result['~']  = true ;
		result['*']  = true ;
		result['\''] = true ;
		result['(']  = true ;
		result[')']  = true ;

		// we will change space to _ ourselves
		result[' ']  = true ;
		return result ;
	}
	static const std::bitset<256> m_map ;

	bool operator()( char t ) const
	{
		return m_map[static_cast<unsigned char>(t)] ;
	}
} ;

template <typename Iterator=std::string::const_iterator>
class UriChar
{
public :
	enum Type { null, alphanum, escape, mark } ;

	UriChar( Iterator begin, Iterator end ) :
		m_begin(begin),
		m_end(end)
	{
	}

	Iterator begin() const
	{
		return m_begin;
	}

	Iterator end() const
	{
		return m_end;
	}

	std::string Str() const
	{
		return std::string(m_begin, m_end) ;
	}

	Type CharType() const
	{
		return
			(m_begin == m_end ? null :
				(*m_begin == '%'
					? (Marked::m_map[static_cast<unsigned char>(operator char())] ? mark : escape)
					: (Marked::m_map[static_cast<unsigned char>(*m_begin)]        ? mark : alphanum)
				)
			);
	}

	operator void*() const
	{
		return m_begin != m_end ? this : 0 ;
	}

	operator char() const
	{
		if ( m_begin == m_end )
			return '\0' ;

		else if ( *m_begin == '%' )
		{
			Iterator i = m_begin;
			long r = std::strtol( std::string(++i, m_end).c_str(), 0, 16 ) ;
			return r >= 0 && r <= std::numeric_limits<unsigned char>::max() ? static_cast<char>(r) : '\0' ;
		}
		
		else
			return *m_begin ;
	}

private :
	Iterator m_begin, m_end ;
} ;

} // end of namespace


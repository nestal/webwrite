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

#include "UriChar.hh"
#include <boost/iterator/iterator_facade.hpp>

namespace wb {

template <typename Iterator=std::string::const_iterator>
class UriCharIterator : public boost::iterator_facade<
	UriCharIterator<Iterator>,
	UriChar<Iterator>,
	std::input_iterator_tag,
	UriChar<Iterator> >
{
public :
	UriCharIterator()
	{
	}

	UriCharIterator( Iterator current, Iterator end ) :
		m_current( current ),
		m_end( end )
	{
	}

private:
	friend class boost::iterator_core_access;

	bool equal( const UriCharIterator<Iterator>& other ) const
	{
		return m_current == other.m_current ;
	}

	UriChar<Iterator> dereference() const
	{
		return UriChar<Iterator>( m_current, Next() ) ;
	}

	void increment()
	{
		m_current = Next() ;
	}

	Iterator Next() const
	{
		Iterator i = m_current ;
		if ( i != m_end )
		{
			if ( *i == '%' )
				Bump(i,2) ;
			Bump(i) ;
		}
		return i ;
	}

	void Bump( Iterator& i, std::size_t n = 1 ) const
	{
		while ( n-- > 0 )
		{
			if ( i != m_end )
				i++ ;
		}
	}

private :
	Iterator	m_current, m_end ;
} ;

} // end of namespace


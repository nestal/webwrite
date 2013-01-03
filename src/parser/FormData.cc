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

#include "FormData.hh"

#include "util/DataStream.hh"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <string>

#include <iostream>

namespace wb {

class StreamReader
{
public :
	StreamReader( DataStream *in ) : m_in(in), m_ptr(m_buf) {}

	std::size_t ReadUntil( DataStream *out, const std::string& target )
	{
		char buf[1024], *ptr = buf;

		while ( true )
		{
			std::size_t size = m_in->Read(ptr, sizeof(buf) - (ptr-buf) ) ;
			if ( size == 0 )
				break ;

			ptr = buf ;

			const char *r = std::search( ptr, ptr+size, target.begin(), target.end() ) ;
			out->Write( ptr, r - ptr ) ;

			if ( r != ptr + size )
			{
			}
		}
	}

	bool Refill()
	{
		// already fill, no need to fill
		if ( CachedSize() == sizeof(m_buf) )
			return true ;

		std::size_t size = m_in->Read( m_ptr, sizeof(m_buf) - CachedSize() ) ;
		return size > 0 ;
	}

	std::size_t CachedSize() const
	{
		return m_ptr - m_buf ;
	}

private :
	DataStream	*m_in ;
	char		m_buf[1024] ;
	char		*m_ptr ;
} ;

/*!	\brief	initialize the form data
*/
FormData::FormData( DataStream *in, const std::string& ctype ) :
	m_in( in )
{
	assert( m_in != 0 ) ;

	
}

void FormData::Save( const fs::path& path )
{
	char buf[1024], *ptr = buf;

	while ( true )
	{
		std::size_t size = m_in->Read(ptr, sizeof(buf) - (ptr-buf) ) ;
		if ( size == 0 )
			break ;

		ptr = buf ;
	}
}

} // end of namespace

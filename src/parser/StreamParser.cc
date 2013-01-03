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

#include "StreamParser.hh"

#include "util/DataStream.hh"

#include <algorithm>
#include <cassert>
#include <cstring>

namespace wb {

StreamParser::StreamParser( DataStream *in ) :
	m_in(in),
	m_end(m_cache)
{
	assert( in != 0 ) ;
}

std::size_t StreamParser::ReadUntil( const std::string& target, DataStream *out )
{
	std::size_t skipped = 0 ;
	
	while ( true )
	{
		// no more input, nothing we can do
		if ( Size() == 0 && !Refill() )
			return skipped ;

		const char *r = std::search( m_cache, m_end, target.begin(), target.end() ) ;
		
		// no matter we found the target or not, we should write the skipped bytes
		// to output
		out->Write( m_cache, r - m_cache ) ;
		skipped += (r - m_cache) ;

		// not found... try again
		if ( r == m_end )
			m_end = m_cache ;
		
		// found! now r points to the chars which is exactly equal to target
		else
		{
			assert( std::string( r, target.size() ) == target ) ;
			skipped += target.size() ;
			
			const char *next	= r + target.size() ;
			std::size_t remain	= Capacity() - (next - m_cache) ;
			std::memmove( m_cache, next, remain ) ;
			m_end = m_cache + remain ;
			
			return skipped ;
		}
	}
}

///	tries to read more data from input and fill the cache.
bool StreamParser::Refill()
{
	// already fill, no need to fill
	if ( Size() == Capacity() )
		return true ;

	std::size_t read = m_in->Read( m_end, Capacity() - Size() ) ;
	m_end += read ;
	return read > 0 ;
}

///	returns the number of bytes stored in the cache
std::size_t StreamParser::Size() const
{
	return m_end - m_cache ;
}

std::size_t StreamParser::Capacity() const
{
	return sizeof(m_cache) ;
}


} // end of namespace

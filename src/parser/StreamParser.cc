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

#include <iostream>

namespace wb {

StreamParser::StreamParser( DataStream *in ) :
	m_in(in),
	m_end(m_cache)
{
	assert( in != 0 ) ;
}

std::size_t StreamParser::ReadUntil( const std::string& target, DataStream *out )
{
	assert( !target.empty() ) ;
	assert( Capacity() >= target.size() ) ;

	std::size_t total = 0 ;
	
	while ( true )
	{
		// no more input, nothing we can do
		if ( Size() == 0 && !Refill() )
			return total ;

		// if Refill() returns true there must be some bytes here
		assert( Size() > 0 ) ;
		
		const char *r = std::find( m_cache, m_end, target[0] ) ;

		// no matter we found the target or not, we should write the skipped bytes
		// to output
		std::size_t skipped = r - m_cache ;
// std::cout << "writing: \"" << std::string( m_cache, skipped ) << "\"" << std::endl ;
		out->Write( m_cache, skipped ) ;
		total += skipped ;

		// not found... try again
		if ( r == m_end )
			m_end = m_cache ;
		
		// found! now see if "r" really points to the chars which is exactly equal to target
		else
		{
			std::size_t remain	= Size() - skipped ;

// std::cout << "before move: m_cache = \"" << std::string( m_cache, m_end ) << "\" remain = " << remain << std::endl ;
			std::memmove( m_cache, r, remain ) ;
			m_end = m_cache + remain ;
// std::cout << "m_cache = \"" << std::string( m_cache, m_end ) << "\"" << std::endl ;

			Refill() ;
// std::cout << "filled: m_cache = \"" << std::string( m_cache, m_end ) << "\"" << std::endl ;
			
			assert( m_cache[0] == target[0] ) ;
			r = std::search( m_cache, m_end, target.begin(), target.end() ) ;
			std::size_t skipped = r - m_cache ;
// std::cout << "writing2: \"" << std::string( m_cache, skipped ) << "\"" << std::endl ;
			out->Write( m_cache, skipped ) ;
			total += skipped ;
			
			if ( r == m_end )
				m_end = m_cache ;
			
			else
			{
// std::cout << "found! " << target.size() << " bytes" << std::endl ;
				const char *next = r + target.size() ;
				remain = Size() - (next - m_cache) ;

				std::memmove( m_cache, next, remain ) ;
				total += target.size() ;
				m_end  = m_cache + remain ;
// std::cout << "found m_cache = \"" << std::string( m_cache, m_end ) << "\"" << std::endl ;
				
				return total ;
			}
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

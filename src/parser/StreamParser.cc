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

std::size_t StreamParser::Read( std::size_t count, DataStream *out )
{
	std::size_t total = 0 ;
	while ( Size() < count && Refill() )
	{
		if ( out != 0 )
			out->Write( m_cache, Size() ) ;
		
		total += Size() ; 
		m_end = m_cache ;
	}
	
	total += Consume( count, out ) ;
	return total ;
}

std::size_t StreamParser::Consume( std::size_t count, DataStream *out )
{
	if ( Size() < count )
		Refill() ;

	std::size_t actual = std::min( Size(), count ) ;

	if ( out != 0 )
		out->Write( m_cache, actual ) ;
	
	std::size_t remain	= Size() - actual ;

	std::memmove( m_cache, m_cache + actual, remain ) ;
	m_end = m_cache + remain ;
	
	return actual ;
}

std::size_t StreamParser::ReadUntil( char target, DataStream *out )
{
	std::size_t total = 0 ;

	while ( true )
	{
		// no more input, nothing we can do
		if ( Size() == 0 && !Refill() )
			return total ;

		// if Refill() returns true there must be some bytes here
		assert( Size() > 0 ) ;
		
		const char *r = std::find( m_cache, m_end, target ) ;
		bool found = ( r != m_end ) ;
		
		// no matter we found the target or not, we should write the skipped bytes
		// to output
		total += Consume( r - m_cache, out ) ;

		// found! now see if "r" really points to the target char
		if ( found )
			break ;
	}
	
	return total ;
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
		
		total += ReadUntil( target[0], out ) ;
		Refill() ;
		
		const char *r = std::search( m_cache, m_end, target.begin(), target.end() ) ;
		bool found = ( r != m_end ) ;
		
		total += Consume( r - m_cache, out ) ;
		
		if ( found )
		{
			total += Consume( target.size(), 0 ) ;
			break ;
		}
	}
	
	return total ;
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

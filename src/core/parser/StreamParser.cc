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

#include <boost/bind.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>
#include "log/Log.hh"

namespace wb {

StreamParser::StreamParser( DataStream *in ) :
	m_in( in ), m_end( m_cache )
{
	assert( in != 0 ) ;

	// first, try to fill up the cache
	Refill( ) ;
}

/**	Copy `count` number of bytes from input to `out`. This function is
	also internally called by ReadUntil() to copy the bytes.
	\param	count	Number of bytes to copy.
	\param	out		Destination DataStream.
	\return			Actually number of bytes copied. This is always
					less than or equal to `count`.
*/
std::size_t StreamParser::Consume( std::size_t count, DataStream *out )
{
	assert( out != 0 ) ;
	std::size_t total = 0 ;

	while ( count > 0 )
	{
		std::size_t actual = std::min( Size( ), count ) ;
		out->Write( m_cache, actual ) ;

		std::size_t remain = Size( ) - actual ;
		std::memmove( m_cache, m_cache + actual, remain ) ;
		m_end = m_cache + remain ;

		total += actual ;
		count -= actual ;

		// we are done when no more bytes
		if ( Size( ) == 0 && !Refill( ) )
			break ;
	}

	return total ;
}

// no need to put the definition in header for private functions
template<typename Find>
StreamParser::Result StreamParser::FindUntil( Find find, DataStream *out )
{
	assert( out != 0 ) ;

	StreamParser::Result result = {} ;

	while ( true )
	{
		// no more input, nothing we can do
		if ( Size( ) == 0 && !Refill( ) )
			break ;

		// if Refill() returns true there must be some bytes here
		assert( Size() > 0 ) ;

		// we must remember the target (i.e. *r) right here and not after
		// calling Consume(), becuase it may overwrite the buffer by calling
		// Refill().
		const char *r = find( m_cache, m_end ) ;
		result.found = ( r != m_end ) ;
		if ( result.found )
			result.target = *r ;

		// no matter we found the target or not, we should write the skipped bytes
		// to output
		result.consumed += Consume( r - m_cache, out ) ;

		// found! now see if "r" really points to the target char
		if ( result.found )
			break ;
	}

	return result ;
}

const char* StreamParser::FindChar(
    const char *begin, const char *end, char target )
{
	return std::find( begin, end, target ) ;
}

StreamParser::Result StreamParser::ReadUntil( char target, DataStream *out )
{
	assert( out != 0 ) ;
	return FindUntil(
	    boost::bind( &StreamParser::FindChar, _1, _2, target ), out ) ;
}

const char* StreamParser::FindAnyChar(
	const char *begin, const char *end, const std::string& target )
{
	return std::find_first_of( begin, end, target.begin(), target.end() ) ;
}

StreamParser::Result StreamParser::ReadUntilAny(
	const std::string& target, DataStream *out )
{
	assert( out != 0 ) ;
	return FindUntil(
	    boost::bind( &StreamParser::FindAnyChar, _1, _2, target ), out ) ;
}

/**	Copy data from input to `out` until the `target` string is found.
	Unlike the single-byte version of ReadUntil(), this one _does_
	not copy the `target` string if it is found. Instead, the `target`
	string will be skipped.

	\param	target	Copy bytes until this string is found in input.
	\param	out		Data will be copied to until `target` is found.
	\return			Parser result. Result::target is not updated.
*/
StreamParser::Result StreamParser::ReadUntil(
    const std::string& target, DataStream *out )
{
	assert( !target.empty() ) ;
	assert( Capacity() >= target.size() ) ;

	Result result = {} ;

	while ( true )
	{
		// no more input, nothing we can do
		if ( Size( ) == 0 && !Refill( ) )
			break ;

		// if Refill() returns true there must be some bytes here
		assert( Size() > 0 ) ;

		result.consumed += ReadUntil( target[0], out ).consumed ;
		Refill( ) ;

		const char *r = std::search(
		    m_cache, m_end, target.begin( ), target.end( ) ) ;
		result.found = ( r != m_end ) ;

		result.consumed += Consume( r - m_cache, out ) ;

		if ( result.found )
		{
			result.consumed += Consume( target.size( ), DevNull() ) ;
			break ;
		}
	}

	return result ;
}

///	tries to read more data from input and fill the cache.
bool StreamParser::Refill( )
{
	// already fill, no need to fill
	if ( Size( ) == Capacity( ) )
		return true ;

	std::size_t read = m_in->Read( m_end, Capacity( ) - Size( ) ) ;
	m_end += read ;
	return read > 0 ;
}

///	returns the number of bytes stored in the cache
std::size_t StreamParser::Size( ) const
{
	return m_end - m_cache ;
}

std::size_t StreamParser::Capacity( ) const
{
	return sizeof( m_cache ) ;
}

} // end of namespace

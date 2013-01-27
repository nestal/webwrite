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

	/// Result of parsing.
	struct Result
	{
		bool		found ;		///< true if the target is found.
		std::size_t	consumed ;	///< # of bytes consumed.
		char		target ;	///< the actual character found.
	} ;

	std::size_t ReadUntil( const std::string& target, DataStream *out ) ;
	Result ReadUntil( char target, DataStream *out ) ;
	Result ReadUntilAny( const std::string& first_of, DataStream *out ) ;
	std::size_t Consume( std::size_t count, DataStream *out = 0 ) ;
		
	bool Refill() ;
	std::size_t Size() const ;
	std::size_t Capacity() const ;

private :
	template <typename Find>
	Result FindUntil( Find find, DataStream *out ) ;
	
	static const char* FindChar( const char *begin, const char *end, char target ) ;
	static const char* FindAnyChar( const char *begin, const char *end,
		const std::string& target ) ;

private :
	DataStream	*m_in ;
	char		m_cache[80] ;
	char		*m_end ;
} ;

} // end of namespace

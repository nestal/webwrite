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

#include "util/Exception.hh"

#include "sha1.h"

#include <cstddef>
#include <iosfwd>
#include <string>

namespace wb {

class Source ;
class Sink ;

class SHA1
{
public :
	static const std::size_t m_size = SHA1HashSize ;

	struct Error : virtual Exception {} ;
	typedef boost::error_info<struct ErrNum, int>	ErrNum_ ;

	struct Val
	{
		unsigned char bytes[m_size] ;
	
		std::string Str() const ;
	} ;

public :
	SHA1( ) ;

	void Input( const void *data, std::size_t size ) ;
	void Reset() ;
	Val Result( ) ;

private :
	SHA1Context	m_ctx ;
} ;

} // end of namespace

namespace std
{
	ostream& operator<<( ostream& os, const wb::SHA1::Val& d ) ;
}

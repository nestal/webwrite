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

#include "Atomic.hh"

#include <boost/cstdint.hpp>

#ifdef WIN32
	#include <windows.h>
#endif

namespace wb {

#ifdef WIN32
template <>
Atomic<long>& Atomic<long>::operator++()
{
	::InterlockedIncrement( &m_ ) ;
	return *this ;
}

template <>
Atomic<long> Atomic<long>::operator++( int )
{
	return ::InterlockedIncrement( &m_ ) ;
}

template <>
Atomic<long>& Atomic<long>::operator+=( long v )
{
	::InterlockedExchangeAdd( &m_, v ) ;
	return *this ;
}
#endif

} // end of namespace

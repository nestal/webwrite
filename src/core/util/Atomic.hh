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

#include <boost/interprocess/detail/atomic.hpp>

namespace wb
{
	template <typename T>
	class Atomic
	{
	public :
		Atomic( T t ) : m_(t) {}

		operator T() const { return m_ ; }

		Atomic& operator++()
#ifdef __GNUC__
		{
			__sync_fetch_and_add( &m_, 1 ) ;
			return *this ;
		}
#else
		;
#endif
		
		Atomic operator++(int)
#ifdef __GNUC__
		{
			return __sync_fetch_and_add( &m_, 1 ) ;
		}
#else
		;
#endif

		Atomic& operator--()
#ifdef __GNUC__
		{
			__sync_fetch_and_sub( &m_, 1 ) ;
			return *this ;
		}
#else
		;
#endif
		Atomic& operator--(int)
#ifdef __GNUC__
		{
			return __sync_fetch_and_sub( &m_, 1 ) ;
		}
#else
		;
#endif

		Atomic& operator+=( T t )
#ifdef __GNUC__
		{
			__sync_fetch_and_add( &m_, t ) ;
			return *this ;
		}
#else
		;
#endif
		
		Atomic& operator-=( T t )
#ifdef __GNUC__
		{
			__sync_fetch_and_sub( &m_, t ) ;
			return *this ;
		}
#else
		;
#endif
	
	private :
		volatile T	m_ ;
	} ;

using namespace boost::interprocess::ipcdetail ;

} // end of namespace

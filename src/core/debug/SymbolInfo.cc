/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2006  Wan Wai Ho

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

#include "SymbolInfo.hh"
#include "Debug.hh"

// glibc backtrace
#include <execinfo.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>

namespace wb { namespace debug {

/// A simple wrapper around glibc backtrace.
std::size_t Backtrace( addr_t *stack, std::size_t count )
{
	return ::backtrace( stack, count ) ;
}

void PrintTrace( const addr_t *stack, std::size_t count, std::ostream& os )
{
	char **func = ::backtrace_symbols(stack, count) ;
	
	for ( std::size_t i = 0 ; i < count && func != 0 && func[i] != 0 ; ++i )
		os << "#"  << i << " [" << stack[i] << "] " << Demangle(func[i]) << std::endl ;
}

} } // end of namespace

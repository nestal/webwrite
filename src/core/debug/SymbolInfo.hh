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

#pragma once

#include "Addr.hh"

#include <memory>
#include <iosfwd>

namespace wb {

///	ource code symbolic information
/**	\internal
	
	This class represents symbolic information about the source code,
	e.g. function names and line numbers. It provides an interface to
	lookup these informations by address.
*/
namespace debug
{
	std::size_t Backtrace( addr_t *stack, std::size_t count ) ;
	void PrintTrace( const addr_t *stack, std::size_t count, std::ostream& os ) ;
	void PrintTrace( const addr_t *stack, std::size_t count, int fd ) ;
} ;

} // end of namespace

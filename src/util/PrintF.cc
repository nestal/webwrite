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

#include "PrintF.hh"

#include "DataStream.hh"

#include <cassert>

namespace wb {

PrintF::PrintF( DataStream *out ) :
	m_out( out )
{
	assert( m_out != 0 ) ;
}

std::size_t PrintF::PutS( const char *str, std::size_t size )
{
	return m_out->Write( str, size ) ;
}

/// Note that the terminating null is not written. In general, there is no
/// need to specially handle null-terminating strings in this project. 
std::size_t PrintF::operator()( const std::string& fmt )
{
	return PutS( fmt.c_str(), fmt.size() ) ;
}

} // end of namespace
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

#include "TeeStream.hh"

#include <cassert>

namespace wb {

TeeStream::TeeStream( DataStream *in, DataStream *out ) :
	m_in( in ),
	m_out( out )
{
	assert( m_in != 0 ) ;
	assert( m_out != 0 ) ;
}

std::size_t TeeStream::Read( char *data, std::size_t size )
{
	std::size_t count = m_in->Read( data, size ) ;
	if ( count > 0 )
		m_out->Write( data, count ) ;
	return count ;
}

std::size_t TeeStream::Write( const char *data, std::size_t size )
{
	return m_out->Write( data, size ) ;
}

} // end of namespace

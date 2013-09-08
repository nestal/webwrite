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

#include "StreamSink.hh"

#include "DataStream.hh"

#include "debug/Assert.hh"

namespace wb {

StreamSink::StreamSink( DataStream *out ) :
	m_out( out )
{
	WB_ASSERT( m_out != 0 ) ;
}

std::streamsize StreamSink::write( const char* s, std::streamsize n )
{
	return m_out->Write( s, static_cast<std::size_t>(n) ) ;
}

} // end of namespace

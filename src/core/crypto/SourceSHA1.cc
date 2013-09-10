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

#include "SourceSHA1.hh"

#include "debug/Assert.hh"

namespace wb {

SourceSHA1::SourceSHA1( Source *in ) : m_in(in)
{
	WB_ASSERT( m_in != 0 ) ;
}

std::streamsize SourceSHA1::read( char *data, std::streamsize size )
{
	std::streamsize count = m_in->read( data, size ) ;
	if ( count > 0 )
		m_sha.Input( data, count ) ;
	
	return count ;
}

SHA1::Digest SourceSHA1::Result()
{
	return m_sha.Result() ;
}

} // end of namespace

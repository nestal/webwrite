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

#include "FormData.hh"

#include "util/DataStream.hh"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <string>

#include <iostream>

namespace wb {

/*!	\brief	initialize the form data
*/
FormData::FormData( DataStream *in, const std::string& ctype ) :
	m_in( in )
{
	assert( m_in != 0 ) ;

	
}

void FormData::Save( const fs::path& path )
{
	char buf[1024], *ptr = buf;

	while ( true )
	{
		std::size_t size = m_in->Read(ptr, sizeof(buf) - (ptr-buf) ) ;
		if ( size == 0 )
			break ;

		ptr = buf ;
	}
}

} // end of namespace
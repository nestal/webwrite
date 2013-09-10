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

#include "SHA1.hh"

#include "util/CArray.hh"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace wb {

SHA1::SHA1( )
{
	Reset() ;
}

void SHA1::Input( const void *data, std::size_t size )
{
	int err = ::SHA1Input( &m_ctx, static_cast<const unsigned char*>(data), size ) ;
	if ( err != 0 )
		BOOST_THROW_EXCEPTION( Error() << ErrNum_(err) ) ;
}

void SHA1::Reset()
{
	::SHA1Reset( &m_ctx ) ;
}

SHA1::Digest SHA1::Result( )
{
	Digest result ;
	::SHA1Result( &m_ctx, result.bytes ) ;
	return result ;
}

std::string SHA1::Digest::Str() const
{
	std::ostringstream ss ;
	ss << *this ;
	return ss.str() ;
}

} // end of namespace

namespace std
{
	ostream& operator<<( ostream& os, const wb::SHA1::Digest& d )
	{
		os << std::hex ;
		for ( int i = 0 ; i < wb::SHA1::m_size ; ++i )
			os << std::setfill('0') << std::setw(2) << static_cast<unsigned>(d.bytes[i]) ;

		return os ;
	}
}

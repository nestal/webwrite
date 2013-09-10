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

#include "util/DataStream.hh"
#include "SHA1.hh"

namespace wb {

class FilterSHA1 : public Source, public Sink
{
public :
	explicit FilterSHA1( Source *in ) ;
	explicit FilterSHA1( Sink *out ) ;
	
	std::streamsize read( char *data, std::streamsize size ) ;
	std::streamsize write( const char *data, std::streamsize size ) ;

	SHA1::Digest Result() ;

private :
	SHA1	m_sha ;
	Source	*m_in ;
	Sink	*m_out ;
} ;

} // end of namespace


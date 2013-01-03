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

#include <cassert>
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
	std::string line ;
	do
	{
		line = m_in->ReadLine( ) ;
		std::cout << " >\"" << line << "\"<" << std::endl ;
		
	} while ( line != "\r\n" && !line.empty() ) ;
}

bool FormData::ReadHyphens( DataStream *out )
{
	char hyphens[2] = {} ;
	if ( m_in->GetChar(hyphens[0]) )
	{
		if ( hyphens[0] != '-' )
		{
			out->Write( &hyphens[0], 1 ) ;
			return false ;
		}

		if ( m_in->GetChar(hyphens[1]) )
		{
			if ( hyphens[1] != '-' )
			{
				out->Write( &hyphens[0], sizeof(hyphens) ) ;
				return false ;
			}
			else
				return true ;
		}
	}
	return false ;
}

} // end of namespace

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

#include "util/FileSystem.hh"

#include "fcgiapp.h"

#include <cstddef>

namespace wb {

class Config ;

class Request
{
public :
	enum Type { file, data } ;

public :
	explicit Request( FCGX_Request *req ) ;

	Type ReqType() const ;
	
	bool IsPost() const ;
	
	std::size_t Recv( char *data, std::size_t size ) ;
	std::size_t Send( const char *data, std::size_t size ) ;
	
	std::size_t SendFile( const fs::path& file, const std::string& mime ) ;
	
private :
	FCGX_Request	*m_req ;
} ;

} // end of namespace

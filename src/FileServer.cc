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

#include "FileServer.hh"

#include "Request.hh"
#include "Resource.hh"

#include <cassert>
#include <iostream>

namespace wb {

FileServer::FileServer( const fs::path& lib_path ) :
	m_lib( lib_path )
{
}

Server* FileServer::Work( Request *req, const Resource& res )
{
	std::string	fname	= res.Filename() ;
	assert( !fname.empty() ) ;
	
	fs::path path = m_lib / ( fname[0] == '_' ? fname.substr(1) : "index.html" ) ;
	std::cout << "serving: " << path << std::endl ;
	
	req->XSendFile( path ) ;
}

} // end of namespace

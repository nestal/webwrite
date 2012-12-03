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

namespace wb {

FileServer::FileServer( const fs::path& lib_path ) :
	m_lib( lib_path )
{
}

Server* FileServer::Work( Request *req, const fs::path& rel )
{
	fs::path file = m_lib / rel ;
	req->PrintF( "X-Sendfile: %s\r\n\r\n", file.string().c_str() ) ;

	return 0 ;
}

} // end of namespace

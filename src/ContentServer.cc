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

#include "ContentServer.hh"

#include "Request.hh"

#include "util/File.hh"

namespace wb {

ContentServer::ContentServer( const fs::path& data_path ) :
	m_path( data_path )
{
}

Server* ContentServer::Work( Request *req, const fs::path& rel )
{
	if ( req->Method() == "POST" && req->Query() == "save" )
	{
		std::string last  = rel.filename().string() ;
		std::string fname = last.substr( 0, last.find('?') ) ;
		
		fs::path file	= m_path / rel.parent_path() / fname ;
		
std::cout << "writing to " << (m_path/rel.parent_path()/fname) << std::endl ;
		
		fs::create_directories( file.parent_path() ) ;
		File f( file, 0600 ) ;
		
		char buf[80] ;
		std::size_t c ;
		while ( (c = req->Recv(buf, sizeof(buf)) ) > 0 )
			f.Write( buf, c ) ;
	}

	return 0 ;
}


} // end of namespace

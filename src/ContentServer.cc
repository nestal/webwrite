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
#include "Resource.hh"
#include "util/DataStream.hh"

#include "util/File.hh"

#include <iostream>

namespace wb {

ContentServer::ContentServer( const fs::path& data_path ) :
	m_path( data_path )
{
}

Server* ContentServer::Work( Request *req, const Resource& res )
{
	std::string	fname	= res.Filename() ;
	fs::path 	file	= res.ContentPath() ;
	
	if ( req->Method() == "POST" && req->Query() == "save" )
	{
std::cout << "writing to " << file << std::endl ;
		
		fs::create_directories( file.parent_path() ) ;
		File f( file, 0600 ) ;
		
		char buf[80] ;
		std::size_t c ;
		while ( (c = req->In()->Read(buf, sizeof(buf)) ) > 0 )
			f.Write( buf, c ) ;
	}
	
	if ( req->Method() == "POST" && req->Query() == "upload" )
	{
		std::string line ;
		do
		{
			line = req->In()->ReadLine( ) ;
			std::cout << " >\"" << line << "\"<" << std::endl ;
		
		} while ( line != "\r\n" && !line.empty() ) ;
	}
	
	else if ( req->Method() == "GET" && req->Query() == "load" )
	{
std::cout << "reading from " << file << std::endl ;
		req->XSendFile( file ) ;
	}

	return 0 ;
}

fs::path ContentServer::LocalPath( const fs::path& rel_url ) const
{
	 return m_path / rel_url ;
}

} // end of namespace

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

#include "ScriptServer.hh"

#include "Request.hh"

#include <boost/format.hpp>

namespace wb {

ScriptServer::ScriptServer()
{
}

Server* ScriptServer::Work( Request *req, const fs::path& location )
{
	if ( location == "var.js" )
	{
		boost::format js( "\r\n\r\nvar post_url = \"%1%\";\n" ) ;
		std::string s = (js % req->Referer() ).str() ;
		
		req->PrintF( "%s", s.c_str() ) ;
	}
	
	return 0 ;
}

} // end of namespace

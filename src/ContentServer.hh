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

#pragma once

#include "Server.hh"

namespace wb {

class Config ;
class Request ;

class ContentServer : public Server
{
public :
	ContentServer( const fs::path& data_path ) ;
	
	Server* Work( Request *req, const fs::path& rel ) ;

	fs::path LocalPath( const fs::path& rel_url ) const ;
	
private :
	fs::path	m_path ;
} ;

} // end of namespace
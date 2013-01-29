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

#include "parser/Json.hh"
#include "util/FileSystem.hh"

#include <string>

namespace wb {

/** 
	\page	Configuration
	
	The configuration file used by WebWrite is based on [JSON](http://www.json.org/).
	This page documents the configuration items.
	
	\include config.json
	
	- name (string)\n
	This is the name of the site. This string will be shown in the upper left corner
	of the page.
	- socket (string)\n
	The socket that WebWrite listens to. This must match with your FastCGI configuration
	of your web server.
*/
namespace cfg
{
	const Json& Inst( const Json& json = Json() ) ;
	
	fs::path Path( const std::string& cfg ) ;
	
	std::string MimeType( const fs::path& file ) ;
} ;

} // end of namespace

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

#include "util/FileSystem.hh"
#include "parser/Query.hh"

#include <boost/function.hpp>

namespace wb {

class Request ;
class Resource ;

class RootServer
{
public :
	RootServer( ) ;
	
	void Work( Request *req, const Resource& res ) ;

private :
	void ServeContent( Request *req, const Resource& res ) ;
	void ServeLibFile( Request *req, const fs::path& path, const std::string& libfile ) ;
	void ServeVar( Request *req ) ;
	void ServeIndex( Request *req, const Resource& res ) ;
	bool ServeDataFile( Request *req, const Resource& res ) ;

	void ServeMimeCss( Request *req, const Resource& res ) ;

private :
	// configuration parameters
	fs::path	m_lib_path ;
	fs::path	m_data_path ;
	
	std::string	m_wb_root ;
	std::string	m_main_page ;
	
	// query string parser
	typedef boost::function<void (RootServer*, Request*, const Resource&)> Handler ;
	Query<Handler>	m_query ;
} ;

} // end of namespace

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
#include "Resource.hh"

#include <boost/function.hpp>

namespace wb {

class Request ;

class RootServer
{
public :
	RootServer( ) ;
	
	void Work( Request *req, const Resource& res ) ;

private :
	// error handlers
	void NotFound( Request *req, const Resource& = Resource() ) ;

	// GET requests
	void DefaultPage( Request *req, const Resource& res ) ;
	void ServeLib( Request *req, const Resource& res ) ;
	void ServeVar( Request *req, const Resource& ) ;
	void ServeIndex( Request *req, const Resource& res ) ;
	void Load( Request *req, const Resource& res ) ;
	void ServeMimeCss( Request *req, const Resource& res ) ;
	
	// POST requests
	void Save( Request *req, const Resource& res ) ;
	void Upload( Request *req, const Resource& res ) ;

	// other helpers
	static std::string GenerateMimeCss( ) ;
	bool ServeFile( Request *req, const fs::path& path, int cache_age = 0 ) ;
	static int ReadOptionalIntConfig( const std::string& base, const std::string& item, int def_value = 0 ) ;

private :
	// configuration parameters
	fs::path	m_lib_redir ;
	fs::path	m_data_path ;
	std::string	m_wb_root ;
	std::string	m_main_page ;
	int			m_lib_cache, m_data_cache, m_index_cache ;

	// pre-generated content
	std::string m_mime_css ;

	// query string parser
	typedef boost::function<void (RootServer*, Request*, const Resource&)> Handler ;
	typedef std::map<std::string, Query<Handler> >	Map ;
	Map	m_srv ;
} ;

} // end of namespace
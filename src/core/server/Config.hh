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
#include <map>

namespace wb {

/**	\brief	Detailed configuration documentation
	
	The configuration file used by WebWrite is based on [JSON](http://www.json.org/).
	This page documents the configuration items.
	
	Configuration File {#config_file}
	==================
	
	Here is a sample of the configuration file:
	
	\include config.json
	
	Web Server Configuration
	========================
	
	WebWrite is a FastCGI back-end. It communicates with the web server using
	FastCGI protocol. 
*/
struct Cfg
{
	/**	This is the name of the site. This string will be shown in the upper 
		left corner of the page. The default value is "WebWrite"
	*/
	std::string		name ;
	
	/**	The socket that WebWrite listens to. This must match with your FastCGI
		configuration of your web server.
	*/
	std::string		socket ;

	/**	This is the path to WebWrite in the URI. If it is "/webwrite", then 
		the URI of WebWrite will be http://example.com/webwrite.
	*/
	std::string		wb_root ;
	
	std::string		main_page ;

	/**	Base URI of the external dependencies, e.g. ckeditor and jquery. If
		jquery is installed at http://example.com/somewhere/jquery-.19.0.min.js, put
		"/somewhere" here.
	*/
	std::string		dep_uri ;

	/**	Number of worker threads lauched by WebWrite. Natural it should be 
		set to the number of CPUs + 2.
	*/
	std::size_t		thread ;

	struct PathCfg
	{
		fs::path	path ;
		fs::path	redir ;
	} ;

	/**	Path configuration for "lib" files. These files are not changed when
		WebWrite is running.
	*/
	PathCfg 	lib ;
	
	/**	Path configuration for "data" files. This is where WebWrite stores the
		the HTML for pages and other files. This is also the main directory
		to backup.
	*/
	PathCfg 	data ;
	
	/**	Path configuration for files storing meta-data. Meta-data includes
		last modification time or information like file owner.
	*/
	PathCfg 	meta ;

	/**	Path configuration for files storing file history. When a page is modified
		the old copy will be moved here.
	*/
	PathCfg 	attic ;
	
	struct Log
	{
		std::string	level ;
		std::string	file ;
	} log ;
	
	typedef std::map<fs::path,	std::string> MimeMap ;
	MimeMap mime ;
	
	static std::string MimeType( const fs::path& ext ) ;
	
	/// Singleton object for easy access everywhere
	static const Cfg& Inst() ;
	
	/// should only call once
	static void Init( const Json& json ) ;
} ;

} // end of namespace

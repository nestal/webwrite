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

/*!	\mainpage

WebWrite is a wiki-like content management system written in HTML5/Javascript/C++.
WebWrite uses [ckeditor](http://ckeditor.com/) for in-place HTML editing. Unlike
other wiki implementation, the pages in WebWrite can be edited with a
what-you-see-is-what-you-get HTML editor. There is no need to learn to type special
wiki-text language like [markdown](http://daringfireball.net/projects/markdown/).

Design Principle
================

WebWrite is designed to be intuitive and simple. Most operations in WebWrite are
designed to be similar to file systems. Resources in WebWrite is organized in a
directory tree like a file system, and they are stored in the server in the same
structure. It is easy for the users and administrators to understand how and
where their data are stored.

Although WebWrite is a dynamic web site, most pages are served as-is without any
transformation in the server side. This ensure the server has minimum loading.
The pages in stored in the server in HTML, which can be directly rendered in the
web browser without any further processing.
  
WebWrite uses [X-Accel-Redirect](http://wiki.nginx.org/XSendfile) to serve static
files. It is currently supported by nginx only (AFAIK). For other web servers,
WebWrite can serve the files by itself, but it would not have the same performance
as using internal re-directs.

Dependencies
============

Required:

- [Boost library](http://boost.org)
- [JSON-C](https://github.com/json-c/json-c)
- [FastCGI development kit](http://www.fastcgi.com/devkit/doc/fcgi-devel-kit.htm)
- [libxml2](http://www.xmlsoft.org/index.html) (optional, but recommended for security)
- [ckeditor](http://ckeditor.com/)


REST API
========

Assumed that webwrite is installed at http://example.com/webwrite/

In general, the URL of webwrite can be decomposed into these parts:
	
    http://example.com/webwrite/some/long/path/to/the/page?load
                                ^-----resource path------^ ^--^-command

Resource
========

A resource is a basic unit of data that can be accessed from in WebWrite. 

The resource path in the URL locates the resource in the server.
*/

#include "FCGIRequest.hh"

#include "server/Config.hh"
#include "server/Resource.hh"
#include "server/RootServer.hh"

#include "log/CompositeLog.hh"
#include "log/DefaultLog.hh"
#include "util/Exception.hh"
#include "util/FileSystem.hh"
#include "util/File.hh"
#include "util/CArray.hh"

#include <boost/exception/all.hpp>

#include <fcgiapp.h>

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <ctime>

using namespace wb ;

void InitLog( )
{
	std::auto_ptr<log::CompositeLog> comp_log(new log::CompositeLog) ;
	LogBase* console_log = comp_log->Add( std::auto_ptr<LogBase>( new log::DefaultLog ) ) ;
	
	// initialize log object
	if ( cfg::Inst().Has("log") )
	{
		std::auto_ptr<LogBase> file_log(new log::DefaultLog( cfg::Inst()["log"]["file"].Str() )) ;
		file_log->Enable( log::debug ) ;
		file_log->Enable( log::verbose ) ;
		file_log->Enable( log::info ) ;
		file_log->Enable( log::warning ) ;
		file_log->Enable( log::error ) ;
		file_log->Enable( log::critical ) ;
		
		// log grive version to log file
		file_log->Log( log::Fmt("webwrite v" VERSION " " __DATE__ " " __TIME__), log::verbose ) ;
		
		comp_log->Add( file_log ) ;
	}
	if ( cfg::Inst()["log"]["level"].Str() == "debug" )
	{
		console_log->Enable( log::verbose ) ;
		console_log->Enable( log::debug ) ;
	}
	else if ( cfg::Inst()["log"]["level"].Str() == "verbose" )
	{
		console_log->Enable( log::verbose ) ;
	}
	
	LogBase::Inst( std::auto_ptr<LogBase>(comp_log.release()) ) ;
}

int main( int argc, char **argv )
{
	try
	{
		cfg::Inst( Json::ParseFile( argc < 2 ? "config.json" : argv[1] ) ) ;
		InitLog( ) ;
		
		RootServer srv ;
		
		FCGX_Request request ;

		FCGX_Init() ;
		FCGX_InitRequest( &request, FCGX_OpenSocket( cfg::Inst()["socket"].Str().c_str(), 0 ), 0 ) ;

		int r ;
		while ( (r = FCGX_Accept_r( &request )) == 0 )
		{
			std::clock_t start = std::clock() ;
		
			FCGIRequest req( &request ) ;
			Log( "requesting: %1%", req.URI(), log::verbose ) ;
			srv.Work( &req, Resource( req.SansQueryURI() ) ) ;
			FCGX_Finish_r( &request ) ;
			
			std::clock_t duration = std::clock() - start ;
			Log( "request finished in %1% milliseconds", duration * 1000.0 / CLOCKS_PER_SEC ) ;
		}
	}
	catch ( Exception& e )
	{
		Log( "exception: %1%", boost::diagnostic_information(e), log::critical ) ;
		throw;
	}

	return 0 ;
}

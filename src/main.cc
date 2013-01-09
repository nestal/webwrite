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

	REST API of webwrite:
	
	Assumed that webwrite is installed at http://example.com/webwrite/
	
	In general, the URL of webwrite can be decomposed into these parts:
	
	\verbatim
http://example.com/webwrite/some/long/path/to/the/page?load
                            ^-----resource path------^ ^--^-command
	\endverbatim
	
	The resource path in the URL locates the resource in the server.
*/

#include "Config.hh"
#include "Request.hh"
#include "Resource.hh"
#include "RootServer.hh"

#include "util/Exception.hh"
#include "util/FileSystem.hh"
#include "util/File.hh"
#include "util/CArray.hh"

#include <boost/exception/all.hpp>

#include "fcgiapp.h"

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <iostream>

int main( int argc, char **argv )
{
	using namespace wb ;
	try
	{
		Config cfg( argc < 2 ? "config.json" : argv[1] ) ;
		RootServer srv( cfg ) ;
		
		FCGX_Request request ;

		FCGX_Init() ;
		FCGX_InitRequest( &request, FCGX_OpenSocket( cfg.Str("socket").c_str(), 0 ), 0 ) ;

		int r ;
		while ( (r = FCGX_Accept_r( &request )) == 0 )
		{
			Request req( &request ) ;
			std::cout << "requesting: " << req.URI() << std::endl ;
			
			srv.Work( &req, Resource( req.SansQueryURI(), cfg ) ) ;

			FCGX_Finish_r( &request ) ;
		}
	}
	catch ( Exception& e )
	{
		std::cerr << boost::diagnostic_information(e) << std::endl ;
	}

	return 0 ;
}

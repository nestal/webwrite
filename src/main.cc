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

#include "Config.hh"
#include "Request.hh"

#include "util/Exception.hh"
#include "util/FileSystem.hh"
#include "util/File.hh"
#include "util/CArray.hh"

#include <boost/exception/all.hpp>

#include "fcgiapp.h"

#include <algorithm>
#include <map>
#include <cstdlib>
#include <cstdio>

using namespace wb ;

typedef std::map<fs::path, std::string> MimeMap ;
const MimeMap::value_type mime_map_val[] =
{
	std::make_pair( fs::path(".html"),	"text/html" ),
	std::make_pair( fs::path(".js"),	"application/javascript" ),
} ;
const MimeMap mime_map( Begin(mime_map_val), End(mime_map_val) ) ;

fs::path RepointPath( const fs::path& req_uri, const Config& cfg )
{
	const fs::path wb_root = cfg.Str("wb-root") ;
	
	std::pair<fs::path::const_iterator, fs::path::const_iterator> r =
		std::mismatch( wb_root.begin(), wb_root.end(), req_uri.begin() ) ;

	fs::path result ;
	while ( r.second != req_uri.end() )
	{
		result /= *r.second ;
		r.second++ ;
	}
	
	return result ;
}

std::size_t SendFile( const fs::path& file, Request *req )
{
	std::cerr << "extension is " << file.extension() << std::endl ;
	MimeMap::const_iterator mm = mime_map.find( file.extension() ) ;
	if ( mm == mime_map.end() )
		throw Exception() ;
	
	return req->SendFile( file, mm->second.c_str() ) ;
}

int main( int argc, char **argv )
{
    if ( argc < 2 )
    {
		std::cerr << "usage: " << argv[0] << " [config file]" << std::endl ;
		return -1 ;
    }

    try
    {
		Config cfg( argv[1] );
		
		FCGX_Request request ;

		FCGX_Init() ;
		FCGX_InitRequest( &request, FCGX_OpenSocket( cfg.Str("socket").c_str(), 0 ), 0 ) ;

		int r = FCGX_Accept_r( &request ) ;
		while ( r == 0 )
		{
			Request req( &request ) ;
		
			std::cerr
				<< "requesting: " << FCGX_GetParam( "REQUEST_URI", request.envp )
				<< std::endl ;

			char buf[80] ;
			int n ;
			do
			{
				n = FCGX_GetStr( buf, sizeof(buf), request.in ) ;
				if ( n > 0 )
				{
					std::fwrite( buf, n, 1, stderr ) ;
				}

			} while ( n == sizeof(buf) ) ;

			fs::path repoint = RepointPath( FCGX_GetParam( "REQUEST_URI", request.envp ), cfg ) ;
			
			if ( !repoint.empty() && *repoint.begin() == "_" )
			{
				fs::path no_ ;
				for ( fs::path::iterator i = ++repoint.begin() ; i != repoint.end() ; ++i )
					no_ /= *i ;
				
				std::cerr << "no_ = " << no_ << std::endl ;
				SendFile( "lib" / no_, &req ) ;
			}
			else
				SendFile( "lib/index.html", &req ) ;
			
			FCGX_Finish_r( &request ) ;
			r = FCGX_Accept_r( &request ) ;
		}
	}
	catch ( Exception& e )
	{
		std::cerr << boost::diagnostic_information(e) << std::endl ;
	}

	return 0 ;
}

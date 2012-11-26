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

#include "util/FileSystem.hh"
#include "util/File.hh"
#include "util/CArray.hh"

#include <algorithm>
#include <map>

#include "fcgiapp.h"
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

fs::path RepointPath( const fs::path& req_uri )
{
	static const fs::path wb_root = "/webwrite/" ;
	
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

std::size_t SendFile( const fs::path& file, FCGX_Stream *out )
{
	File f( file ) ;
	
	std::cerr << "extension is " << file.extension() << std::endl ;
	MimeMap::const_iterator mm = mime_map.find( file.extension() ) ;
	if ( mm == mime_map.end() )
		throw Exception() ;
	
	// content type header
	FCGX_FPrintF( out,
		"Content-type: %s\r\n"
		"\r\n",
		mm->second.c_str() ) ;
	
	char buf[4*1024] ;
	std::size_t count ;
	while ( (count = f.Read( buf, sizeof(buf) )) > 0 )
	{
		FCGX_PutStr( buf, count, out ) ;
	}
	
	return 0 ;
}

int main(void)
{
    int count = 0;

	FCGX_Request request ;

	FCGX_Init() ;
	FCGX_InitRequest( &request, FCGX_OpenSocket( ":9000", 0 ), 0 ) ;

	int r = FCGX_Accept_r( &request ) ;
    while ( r == 0 )
	{
		std::cerr
			<< "requesting: " << FCGX_GetParam( "REQUEST_URI", request.envp )
			<< std::endl ;

		char **env = request.envp;
		for ( int i = 0 ; env[i] != 0 ; i++ )
		{
//			std::cout << env[i] << std::endl ;
		}
	
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

		fs::path repoint = RepointPath( FCGX_GetParam( "REQUEST_URI", request.envp ) ) ;
		
		if ( !repoint.empty() && *repoint.begin() == "_" )
		{
			fs::path no_ ;
			for ( fs::path::iterator i = ++repoint.begin() ; i != repoint.end() ; ++i )
				no_ /= *i ;
			
			std::cerr << "no_ = " << no_ << std::endl ;
			SendFile( "lib" / no_, request.out ) ;
		}
		else
			SendFile( "lib/index.html", request.out ) ;
		
		FCGX_Finish_r( &request ) ;
		r = FCGX_Accept_r( &request ) ;
	}

	fprintf( stderr, "quit" ) ;

	return 0 ;
}

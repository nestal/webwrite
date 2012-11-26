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

#include "fcgiapp.h"
#include <cstdlib>
#include <cstdio>

using namespace wb ;

std::size_t SendFile( const fs::path& file, FCGX_Stream *out )
{
	File f( file ) ;
	
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

        FCGX_FPrintF( request.out,
				"Content-type: text/html\r\n"
                "\r\n"
/*                "<title>FastCGI Hello!</title>"
                "<h1>FastCGI Hello!</h1>"
                "Request number %d running on host <i>%s</i>\n",
                ++count, FCGX_GetParam( "SERVER_NAME", request.envp )*/ );

		char **env = request.envp;
		for ( int i = 0 ; env[i] != 0 ; i++ )
		{
//			FCGX_FPrintF( request.out, "<p>%s</p>\n", env[i] ) ;
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

		SendFile( "lib/index.html", request.out ) ;
		
		FCGX_Finish_r( &request ) ;
		r = FCGX_Accept_r( &request ) ;
	}

	fprintf( stderr, "quit" ) ;

	return 0 ;
}

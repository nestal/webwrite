#include "fcgiapp.h"
#include <stdlib.h>
#include <stdio.h>

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
                "<title>FastCGI Hello!</title>"
                "<h1>FastCGI Hello!</h1>"
                "Request number %d running on host <i>%s</i>\n",
                ++count, FCGX_GetParam( "SERVER_NAME", request.envp ) );

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
				fwrite( buf, n, 1, stderr ) ;
			}

		} while ( n == sizeof(buf) ) ;

		FCGX_Finish_r( &request ) ;
		r = FCGX_Accept_r( &request ) ;
	}

	fprintf( stderr, "quit" ) ;

	return 0 ;
}

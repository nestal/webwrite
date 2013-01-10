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

#include "log/CompositeLog.hh"
#include "log/DefaultLog.hh"
#include "util/Exception.hh"
#include "util/FileSystem.hh"
#include "util/File.hh"
#include "util/CArray.hh"

#include <boost/exception/all.hpp>

#include "fcgiapp.h"

#include <algorithm>
#include <cstdlib>
#include <cstdio>

using namespace wb ;

void InitLog( const Config& cfg )
{
	std::auto_ptr<log::CompositeLog> comp_log(new log::CompositeLog) ;
	LogBase* console_log = comp_log->Add( std::auto_ptr<LogBase>( new log::DefaultLog ) ) ;
	
	// initialize log object
	if ( cfg.Get().Has("log") )
	{
		std::auto_ptr<LogBase> file_log(new log::DefaultLog( cfg.Get()["log"]["file"].Str() )) ;
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
	if ( cfg.Get()["log"]["level"].Str() == "debug" )
	{
		console_log->Enable( log::verbose ) ;
		console_log->Enable( log::debug ) ;
	}
	else if ( cfg.Get()["log"]["level"].Str() == "verbose" )
	{
		console_log->Enable( log::verbose ) ;
	}
	
	LogBase::Inst( std::auto_ptr<LogBase>(comp_log.release()) ) ;
}

int main( int argc, char **argv )
{
	try
	{
		Config cfg( argc < 2 ? "config.json" : argv[1] ) ;
		InitLog( cfg ) ;
		
		RootServer srv( cfg ) ;
		
		FCGX_Request request ;

		FCGX_Init() ;
		FCGX_InitRequest( &request, FCGX_OpenSocket( cfg.Str("socket").c_str(), 0 ), 0 ) ;

		int r ;
		while ( (r = FCGX_Accept_r( &request )) == 0 )
		{
			Request req( &request ) ;
			Log( "requesting: %1%", req.URI(), log::verbose ) ;
			
			srv.Work( &req, Resource( req.SansQueryURI(), cfg ) ) ;

			FCGX_Finish_r( &request ) ;
		}
	}
	catch ( Exception& e )
	{
		Log( "exception: %1%", boost::diagnostic_information(e), log::critical ) ;
	}

	return 0 ;
}

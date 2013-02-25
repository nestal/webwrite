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
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

#include <fcgiapp.h>

#include <algorithm>
#include <cstdlib>
#include <cstdio>

using namespace wb ;

void InitLog( )
{
	std::auto_ptr<log::CompositeLog> comp_log(new log::CompositeLog) ;
	LogBase* console_log = comp_log->Add( std::auto_ptr<LogBase>( new log::DefaultLog ) ) ;
	
	// initialize log object
	if ( !Cfg::Inst().log.file.empty() )
	{
		std::auto_ptr<LogBase> file_log(new log::DefaultLog( Cfg::Inst().log.file )) ;
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
	if ( Cfg::Inst().log.level == "debug" )
	{
		console_log->Enable( log::verbose ) ;
		console_log->Enable( log::debug ) ;
	}
	else if ( Cfg::Inst().log.level == "verbose" )
	{
		console_log->Enable( log::verbose ) ;
	}
	
	LogBase::Inst( std::auto_ptr<LogBase>(comp_log.release()) ) ;
}

void Thread( int sock, RootServer *srv, boost::mutex *mutex )
{
	FCGX_Request request ;
	FCGX_InitRequest( &request, sock, 0 ) ;

	while ( true )
	{
		mutex->lock() ;
		int r = FCGX_Accept_r( &request ) ;
		mutex->unlock() ;
		if ( r < 0 )
		{
			Log( "FCGX_Accept_r() error %1%", r ) ;
			break ;
		}
		else
		{
			// make sure FCGIRequest destroys before calling FCGX_Finish_r()
			FCGIRequest req( &request ) ;
			srv->Work( &req, Resource( req.SansQueryURI() ) ) ;
		}
		FCGX_Finish_r( &request ) ;
	}
}

int main( int argc, char **argv )
{
	try
	{
		File cfg_file( argc < 2 ? "config.json" : argv[1] ) ;
		Cfg::Init( Json::Parse( &cfg_file ) ) ;
		InitLog( ) ;
		
		FCGX_Init() ;
		int sock = FCGX_OpenSocket( Cfg::Inst().socket.c_str(), 0 ) ;

		std::vector<boost::thread*> threads ;

		RootServer srv ;
		boost::mutex accept_mutex ;

		for ( std::size_t i = 0 ; i < Cfg::Inst().thread ; i++ )
		{
			boost::thread *t = new boost::thread(boost::bind( &Thread, sock, &srv, &accept_mutex )) ;
			threads.push_back( t ) ;
		}
		
		for ( std::vector<boost::thread*>::iterator i = threads.begin() ;
			i != threads.end() ; ++i )
		{
			(*i)->join() ;
			delete *i ;
		}
	}
	catch ( Exception& e )
	{
		Log( "exception: %1%", boost::diagnostic_information(e), log::critical ) ;
		throw;
	}

	return 0 ;
}

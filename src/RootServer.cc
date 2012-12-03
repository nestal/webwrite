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

#include "RootServer.hh"

#include "Config.hh"
#include "Request.hh"

namespace wb {

RootServer::RootServer( const Config& cfg ) :
	m_file( cfg.Base() ),
	m_wb_root( cfg.Str("wb-root") )
{
}
	
Server* RootServer::Work( Request *req, const fs::path& location ) 
{
	fs::path rel = Relative( location ) ;
	
	if ( !rel.empty() && rel.begin()->string().front() == '_' )
	{
		return m_file.Work( req, rel.string().substr(1) ) ;
	}
	else
	{
		return m_file.Work( req, "lib/index.html" ) ;
	}
	
	return 0 ;
}

fs::path RootServer::Relative( const fs::path& loc ) const
{
	std::pair<fs::path::const_iterator, fs::path::const_iterator> r =
		std::mismatch( m_wb_root.begin(), m_wb_root.end(), loc.begin() ) ;

	fs::path result ;
	while ( r.second != loc.end() )
	{
		result /= *r.second ;
		r.second++ ;
	}
	
	return result ;
}

} // end of namespace

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

#include "FormData.hh"

#include "parser/StreamParser.hh"
#include "util/File.hh"
#include "util/StringStream.hh"
#include "util/Exception.hh"

#include <boost/regex.hpp>

#include <cassert>
#include <string>

#include <iostream>

namespace wb {

/*!	\brief	initialize the form data
*/
FormData::FormData( DataStream *in, const std::string& ctype ) :
	m_in( in )
{
	assert( m_in != 0 ) ;
	
	static const boost::regex e( ".*; *boundary=(.+)" ) ;
	boost::smatch m ;
	if ( boost::regex_match( ctype, m, e ) )
		m_boundary = m[1] ;
}

void FormData::Save( const fs::path& path )
{
	StreamParser p( m_in ) ;

	StringStream boundary ;
	p.ReadUntil( "\r\n", &boundary ) ;
	if ( boundary.Str() != ( "--" + m_boundary ) )
		BOOST_THROW_EXCEPTION(
			Exception( )
				<< expt::ErrMsg("invalid boundary")
				<< ExpectedBoundary( m_boundary )
				<< ActualBoundary( boundary.Str() )
		) ;
	
	StringStream headers ;
	p.ReadUntil( "\r\n\r\n", &headers ) ;

	std::string fname = "outfile", hstr = headers.Str() ;
	static const boost::regex e( ".*filename=\"(.+)\"" ) ;
	boost::smatch m ;
	if ( boost::regex_search( hstr, m, e ) )
		fname = m[1] ;

	File f( path / fname, 0600 ) ;
	p.ReadUntil( "\r\n" + boundary.Str(), &f ) ;
}

} // end of namespace

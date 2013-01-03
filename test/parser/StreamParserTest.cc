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

#include "StreamParserTest.hh"
#include "Assert.hh"

#include "parser/StreamParser.hh"
#include "util/StringStream.hh"

namespace wbut {

using namespace wb ;

StreamParserTest::StreamParserTest( )
{
}

void StreamParserTest::Test( )
{
	StringStream input( "line 1\r\nline 2--something--\r\n" ), output ;
	StreamParser subject( &input ) ;
	
	WBUT_ASSERT_EQUAL( subject.ReadUntil( "\r\n", &output ), sizeof("line 1\r\n")-1 ) ;
	WBUT_ASSERT_EQUAL( output.Str(), "line 1" ) ;

	output.Str("") ;
	
	WBUT_ASSERT_EQUAL(
		subject.ReadUntil( "--something--", &output ),
		sizeof("line 2--something--")-1 ) ;
		
	WBUT_ASSERT_EQUAL( output.Str(), "line 2" ) ;

}

} // end of namespace

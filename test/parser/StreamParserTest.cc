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

#include "xml/HtmlValidator.hh"
#include "parser/StreamParser.hh"
#include "util/StringStream.hh"
#include "util/File.hh"

#include <boost/test/unit_test.hpp>

using namespace wb ;

namespace
{
	// fixture
	struct F
	{
		F()
		{
		}
	} ;
}

BOOST_FIXTURE_TEST_SUITE( StreamParserTest, F )

BOOST_AUTO_TEST_CASE( TestConsume )
{
	StringStream input( "12345" ), output ;
	StreamParser subject( &input ) ;
	
	BOOST_CHECK_EQUAL( subject.Consume( 3, &output ), 3 ) ;
	BOOST_CHECK_EQUAL( output.Str(), "123" ) ;
}

BOOST_AUTO_TEST_CASE( TestConsume2 )
{
	StringStream input( "12345" ), output ;
	StreamParser subject( &input ) ;
	
	BOOST_CHECK_EQUAL( subject.Consume( 100, &output ), 5 ) ;
	BOOST_CHECK_EQUAL( output.Str(), "12345" ) ;
}

BOOST_AUTO_TEST_CASE( TestChar )
{
	StringStream input( "line 1$#line 2--something--???###" ), output ;
	StreamParser subject( &input ) ;
	
	BOOST_CHECK_EQUAL( subject.ReadUntil( "1", &output ), 6 ) ;
	BOOST_CHECK_EQUAL( output.Str(), "line " ) ;
}

BOOST_AUTO_TEST_CASE( TestString )
{
	StringStream input( "line 1$#line 2--something--???###" ), output ;
	StreamParser subject( &input ) ;
	
	BOOST_CHECK_EQUAL( subject.ReadUntil( "$#", &output ), sizeof("line 1$#")-1 ) ;
	BOOST_CHECK_EQUAL( output.Str(), "line 1" ) ;

	output.Str("") ;
	
	BOOST_CHECK_EQUAL(
		subject.ReadUntil( "--something--", &output ),
		sizeof("line 2--something--")-1 ) ;
		
	BOOST_CHECK_EQUAL( output.Str(), "line 2" ) ;

}

BOOST_AUTO_TEST_CASE( TestFile )
{
	File input( TEST_DATA "textures.form" ) ;
	StringStream output ;
	
	StreamParser subject( &input ) ;
	
	std::string boundary( "-----------------------------11351845291583120309948566114" ) ;
	BOOST_CHECK_EQUAL( subject.ReadUntil( "\r\n", &output ), boundary.size() + 2 ) ;
	BOOST_CHECK_EQUAL( output.Str(), boundary ) ;
	
	// skip content-disposition. too long to check
	output.Str("") ;
	subject.ReadUntil( "\r\n", &output ) ;
	
	// content-type
	output.Str("") ;
	subject.ReadUntil( "\r\n\r\n", &output ) ;
	BOOST_CHECK_EQUAL( output.Str(), "Content-Type: image/jpeg" ) ;
	
	// extract the file
	File first( "brick.jpg", 0600 ) ;
	subject.ReadUntil( "\r\n" + boundary, &first ) ;
}

BOOST_AUTO_TEST_SUITE_END()

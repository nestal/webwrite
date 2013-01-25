/*
	webwrite: an GPL wiki-like website with in-place editing
	Copyright (C) 2013 Wan Wai Ho

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	MA  02110-1301, USA.
*/

#include "parser/xml/HtmlValidator.hh"
#include "util/StringStream.hh"

#define BOOST_TEST_MODULE HtmlValidatorTest
#include <boost/test/unit_test.hpp>

using namespace wb ;

BOOST_AUTO_TEST_CASE( HtmlValidatorTest )
{
	StringStream ss ;
    HtmlValidator subject( &ss ) ;

    const char html[] = "<html><body><div class=\"wow\">hi&gt;</body><div/></html>" ;
    subject.Write( html, sizeof(html)-1 ) ;

    std::cout << ss.Str() << std::endl ;

    BOOST_CHECK( true );
}


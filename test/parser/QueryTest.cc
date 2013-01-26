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

#include "parser/Query.hh"

#include <boost/test/unit_test.hpp>

using namespace wb ;

namespace
{
	struct F
	{
	} ;
}

BOOST_FIXTURE_TEST_SUITE( QueryTest, F )

BOOST_AUTO_TEST_CASE( TestNormalParse )
{
	Query<int> subject ;
	subject.Add( "name", 1 ) ;
	subject.Add( "value", 2 ) ;
	BOOST_CHECK_EQUAL( subject.Parse( "name=value" ), 1 ) ;
	BOOST_CHECK_EQUAL( subject.Parse( "value=name" ), 2 ) ;
	BOOST_CHECK_EQUAL( subject.Parse( "2=1" ), 0 ) ;
}

BOOST_AUTO_TEST_CASE( TestEmpty )
{
	Query<int> subject( 1000 ) ;
	subject.Add( "",	0 ) ;
	subject.Add( "two",	2 ) ;
	BOOST_CHECK_EQUAL( subject.Parse( "" ), 0 ) ;
	BOOST_CHECK_EQUAL( subject.Parse( "two#" ), 2 ) ;
	BOOST_CHECK_EQUAL( subject.Parse( "three" ), 1000 ) ;
	
}

BOOST_AUTO_TEST_SUITE_END()

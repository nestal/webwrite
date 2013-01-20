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

#include "QueryTest.hh"
#include "parser/Query.hh"

#include "Assert.hh"

#include <boost/bind.hpp>

namespace wbut {

using namespace wb ;

QueryTest::QueryTest( )
{
}

void QueryTest::Test( )
{
	Query<int> subject ;
	subject.Add( "name", 1 ) ;
	subject.Add( "value", 2 ) ;
	WBUT_ASSERT_EQUAL( subject.Parse( "name=value" ), 1 ) ;
	WBUT_ASSERT_EQUAL( subject.Parse( "value=name" ), 2 ) ;
	WBUT_ASSERT_EQUAL( subject.Parse( "2=1" ), 0 ) ;
}

void QueryTest::TestEmpty()
{
	Query<int> subject( 1000 ) ;
	subject.Add( "",	0 ) ;
	subject.Add( "two",	2 ) ;
	WBUT_ASSERT_EQUAL( subject.Parse( "" ), 0 ) ;
	WBUT_ASSERT_EQUAL( subject.Parse( "two#" ), 2 ) ;
	WBUT_ASSERT_EQUAL( subject.Parse( "three" ), 1000 ) ;
	
}

} // end of namespace

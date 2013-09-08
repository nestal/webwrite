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

#include "xml/Doc.hh"
#include <boost/test/unit_test.hpp>

using namespace xml ;

namespace
{
	struct F
	{
	} ;
}

BOOST_FIXTURE_TEST_SUITE( DocTest, F )

BOOST_AUTO_TEST_CASE( Test )
{
	Doc a( TEST_DATA "hihi.xml" ) ;
	CNode b = a["hihi"] ;
	
	BOOST_CHECK_EQUAL( b.ParentDoc(), &a ) ;
}

BOOST_AUTO_TEST_SUITE_END()

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

#include "util/File.hh"
#include <boost/test/unit_test.hpp>

using namespace wb ;

namespace
{
	struct F
	{
	} ;
}

BOOST_FIXTURE_TEST_SUITE( FileTest, F )

BOOST_AUTO_TEST_CASE( TestRandom )
{
	File f ;
	fs::path p = f.OpenRandom( "tmp-file-XXXXXX" ) ;
	std::cout << p << std::endl ;
}

BOOST_AUTO_TEST_SUITE_END()

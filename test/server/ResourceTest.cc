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

#include "server/Resource.hh"
#include "parser/Json.hh"
#include "server/Config.hh"

#include <boost/test/unit_test.hpp>

using namespace wb ;

namespace
{
	struct F
	{
		F()
		{
			Json cfg ;
			cfg.Add( "wb_root", Json("/webwrite") ) ;
			
			cfg.Add( "lib",   Json::Parse( "{\"path\":\"lib\",  \"redir\":\"/b/lib\"}" ) ) ;
			cfg.Add( "data",  Json::Parse( "{\"path\":\"data\",  \"redir\":\"/b/data\"}" ) ) ;
			cfg.Add( "meta",  Json::Parse( "{\"path\":\"meta\",  \"redir\":\"/b/meta\"}" ) ) ;
			cfg.Add( "attic", Json::Parse( "{\"path\":\"attic\", \"redir\":\"/b/attic\"}" ) ) ;
			
			cfg.Add( "log", Json() ) ;
			cfg.Add( "mime", Json() ) ;
			
			Cfg::Init( cfg ) ;
		}
	} ;
}

BOOST_FIXTURE_TEST_SUITE( ResourceTest, F )

BOOST_AUTO_TEST_CASE( Test )
{
	Resource a;
}

BOOST_AUTO_TEST_SUITE_END()

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

#include "xml/HTMLStreamFilter.hh"
#include "util/StringStream.hh"

#include <boost/test/unit_test.hpp>

using namespace wb ;

namespace
{
	// fixture
	struct F
	{
		StringStream ss ;
	    HTMLStreamFilter subject ;
		
		F() :
			subject( &ss )
		{
		}
	} ;
}

BOOST_FIXTURE_TEST_SUITE( HTMLStreamFilterTest, F )

BOOST_AUTO_TEST_CASE( TestFilterScript )
{
    const char html[] = "<html><body><div>hello&amp;</div><script class=\"wow\">hi&gt;</script></body></html>" ;
    subject.Write( html, sizeof(html)-1 ) ;

    std::string exp = "<div>hello&amp;</div>" ;

    BOOST_CHECK_EQUAL( ss.Str(), exp );
}

BOOST_AUTO_TEST_CASE( TestFilterChildren )
{
    const char html[] =
    	"<html><body>"
    	"<object class=\"wow\"><p>should be ignored</p><span>oops&gt;</span></object>"
    	"<div>I am not ignored!&gt;more chars</div>"
    	"</body></html>" ;
    subject.Write( html, sizeof(html)-1 ) ;

    std::string exp = "<div>I am not ignored!&gt;more chars</div>" ;

    BOOST_CHECK_EQUAL( ss.Str(), exp );
}

BOOST_AUTO_TEST_CASE( TestEntities )
{
    const char html[] =
    	"<html><body>"
    	"<div>many entities like: &something; should be the same</div>"
    	"</body></html>" ;
    subject.Write( html, sizeof(html)-1 ) ;

    std::string exp = "<div>many entities like: &amp;something; should be the same</div>" ;
    BOOST_CHECK_EQUAL( ss.Str(), exp );
}

BOOST_AUTO_TEST_CASE( TestParseTag )
{
    const char html[] =
    	"<html><body>"
    	"<div>many entities like: &something; should be the same</div>"
    	"<script>??<<>><<?&&& this should be skipped ####</script>"
    	"</body></html>" ;

    StringStream in( html ), out ;
    subject.Parse( &in, &out ) ;
    
    const char exp[] =
    	"<html><body>"
    	"<div>many entities like: &something; should be the same</div>"
    	"</body></html>" ;
    
    BOOST_CHECK_EQUAL( out.Str(), exp ) ;
}

BOOST_AUTO_TEST_SUITE_END()

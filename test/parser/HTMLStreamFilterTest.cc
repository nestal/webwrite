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

#include "parser/HTMLStreamFilter.hh"
#include "util/StringStream.hh"

#include <boost/test/unit_test.hpp>

using namespace wb ;

namespace
{
	// fixture
	struct F
	{
	    HTMLStreamFilter subject ;
	} ;
}

BOOST_FIXTURE_TEST_SUITE( HTMLStreamFilterTest, F )

BOOST_AUTO_TEST_CASE( TestFilterScript )
{
    const char html[] = "<html><body><div>hello&amp;</div><script class=\"wow\">hi&gt;</script></body></html>" ;

	StringStream in( html ), out ;
	subject.Parse( &in, &out ) ;
    
    const char exp[] =
    	"<html><body><div>hello&amp;</div></body></html>" ;
    	
    BOOST_CHECK_EQUAL( out.Str(), exp ) ;
}

BOOST_AUTO_TEST_CASE( TestFilterChildren )
{
    const char html[] =
    	"<html><body>"
    	"<object class=\"wow\"><p>should be ignored</p><span>oops&gt;</span></object>"
    	"<div>I am not ignored!&gt;more chars</div>"
    	"</body></html>" ;
	StringStream in( html ), out ;
	subject.Parse( &in, &out ) ;

	std::string exp = "<html><body><div>I am not ignored!&gt;more chars</div></body></html>" ;
	
	BOOST_CHECK_EQUAL( out.Str(), exp );
}

BOOST_AUTO_TEST_CASE( TestEntities )
{
    const char html[] =
    	"<html><body>"
    	"<div>many entities like: &something; should be the same</div>"
    	"</body></html>" ;
	StringStream in( html ), out ;
	subject.Parse( &in, &out ) ;

	std::string exp = "<html><body><div>many entities like: &something; should be the same</div></body></html>" ;
	BOOST_CHECK_EQUAL( out.Str(), exp );
}

BOOST_AUTO_TEST_CASE( TestParseTag )
{
    const char html[] =
    	"<html><body>"
    	"<div>many entities like: &something; should be the same</div>"
    	"<script>?? <<>><<?&&& this should be skipped ####</script>"
    	"</body></html>" ;

    StringStream in( html ), out ;
    subject.Parse( &in, &out ) ;
    
    const char exp[] =
    	"<html><body>"
    	"<div>many entities like: &something; should be the same</div>"
    	"</body></html>" ;
    
    BOOST_CHECK_EQUAL( out.Str(), exp ) ;
}

BOOST_AUTO_TEST_CASE( TestSkipBadEndTag )
{
    const char html[] =
    	"<html><body>"
    	"<div>many entities like: &something; should be the same</div>"
    	"</badtag>this should not be skipped"
    	"<href> skippied </href>"
    	"</body></html>" ;

    StringStream in( html ), out ;
    subject.Parse( &in, &out ) ;
    
    const char exp[] =
    	"<html><body>"
    	"<div>many entities like: &something; should be the same</div>"
    	"this should not be skipped"
    	"</body></html>" ;
    
    BOOST_CHECK_EQUAL( out.Str(), exp ) ;
}

BOOST_AUTO_TEST_SUITE_END()

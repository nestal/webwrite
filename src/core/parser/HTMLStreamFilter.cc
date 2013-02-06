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

#include "HTMLStreamFilter.hh"
#include "StreamParser.hh"

#include "log/Log.hh"
#include "util/CArray.hh"
#include "util/DataStream.hh"
#include "util/PrintF.hh"
#include "util/StringStream.hh"

#include <cassert>
#include <set>

namespace wb {

namespace
{
	// hard code the HTML element white list for now
	const std::string white_list_array[] =
	{
		"html",	"a",	"img",		"sub",
		"body",	"br",	"pre",		"sup",
		"div",	"b",	"strong",
		"span",	"u",	"em",
		"p",	"tt",
		
		"h1",	"ul",	"dd",	"table",
		"h2",	"ol",			"tbody",
		"h3",	"li",			"tr",
		"h4",	"dl",			"td",
		"h5",	"dt",
	} ;

	const std::set<std::string> white_list(
		Begin(white_list_array),
		End(white_list_array) ) ;

	const std::string ignore_list_array[] =
	{
		"html", "body",
	} ;
	
	const std::set<std::string> ignore_list(
		Begin(ignore_list_array),
		End(ignore_list_array) ) ;
	
	const std::string self_close_array[] =
	{
		"br",	"hr",	"img"
	} ;
	
	const std::set<std::string> self_close(
		Begin(self_close_array),
		End(self_close_array) ) ;
	
} // end of local namespace

HTMLStreamFilter::HTMLStreamFilter()
{
}

/// Parsed according to W3C specification: http://www.w3.org/TR/html5/syntax.html
void HTMLStreamFilter::Parse( DataStream *in, DataStream *out )
{
	StreamParser inp( in ) ;
	
	// throw away stuff until first open tag
	StreamParser::Result r = inp.ReadUntil( '<', DevNull() ) ;
	
	while ( r.found )
	{
		// throw away the '<'
		assert( r.target == '<' ) ;
		inp.Consume(1) ;
		
		StringStream tag ;
		r = inp.ReadUntilAny( " >\n", &tag ) ;

		bool end_tag	= !tag.Str().empty() && tag.Str()[0] == '/' ;
		bool good		= CheckElement( end_tag ? tag.Str().substr(1) : tag.Str() ) ;
		
		// if the element is in the white list, we consider it filtered in
		// and scan for next element
		if ( good )
		{
			std::string element = '<' + tag.Str() ;
			out->Write( element.c_str(), element.size() ) ;
		
			// tag not closed yet. read its attributes
			if ( r.target == ' ' )
				inp.ReadUntil( '>', out ) ;
		}
		
		else if ( tag.Str() == "!--" )
		{
			Log( "comments?" ) ;
			inp.ReadUntil( "-->", DevNull() ) ;
		}
		
		// element not in the white list. we will skip everything inside it
		// until its end tag
		else
		{
			// tag not closed yet. skip its attributes
			if ( r.target == ' ' )
				inp.ReadUntil( '>', DevNull() ) ;

			// if the bad tag is a start tag, skip everything until we see
			// the end tag
			if ( !end_tag )
				inp.ReadUntil( "</" + tag.Str(), DevNull() ) ;
			
			// skip close tag
			if ( inp.ReadUntil( '>', DevNull() ).found )
				inp.Consume(1) ;
		}
		
		r = inp.ReadUntil( '<', out ) ;
	}
}

/// List of HTML5 tags can be omitted:
/// http://www.w3.org/TR/html5/syntax.html#syntax-tag-omission
bool HTMLStreamFilter::CheckElement( const std::string& element )
{
	if ( white_list.find(element) == white_list.end() )
	{
		Log( "read invalid tag %1%", element ) ;
		return false ;
	}
	else
		return true ;
}

} // end of namespace

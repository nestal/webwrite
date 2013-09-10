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

#include "HtmlDoc.hh"
#include "debug/Assert.hh"

#include <libxml/HTMLparser.h>

namespace xml {

HtmlDoc::HtmlDoc( const std::string& fname ) :
	Doc( reinterpret_cast<xmlDocPtr>(::htmlReadFile(
		fname.c_str(),
		"UTF-8",
		HTML_PARSE_RECOVER   | HTML_PARSE_PEDANTIC  |
		HTML_PARSE_NONET     | HTML_PARSE_COMPACT	|
		HTML_PARSE_NOERROR ) )
	)
{
}

HtmlDoc::HtmlDoc( wb::Source *src ) :
	Doc( reinterpret_cast<xmlDocPtr>(::htmlReadIO(
		Doc::ReadCallback,
		Doc::CloseCallback,
		src,
		0,
		"UTF-8",
		HTML_PARSE_RECOVER   | HTML_PARSE_PEDANTIC  |
		HTML_PARSE_NONET     | HTML_PARSE_COMPACT	|
		HTML_PARSE_NOERROR ) )
	)
{
}

HtmlDoc::HtmlDoc( const HtmlDoc& rhs ) :
	Doc( rhs )
{
	WB_ASSERT( Check() ) ;
}

HtmlDoc::~HtmlDoc()
{
	WB_ASSERT( Check() ) ;
}

void HtmlDoc::SaveContent( const std::string& fname ) const
{
}

} // end of namespace

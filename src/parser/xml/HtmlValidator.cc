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

#include "HtmlValidator.hh"

#include "log/Log.hh"
#include "util/CArray.hh"

#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>

#include <cassert>
#include <set>

namespace wb {

struct HtmlValidator::Impl
{
	htmlParserCtxtPtr	ctx ;
	DataStream			*out ;
} ;

namespace {

// hard code the HTML element white list for now
const std::string white_list_array[] =
{
	"html",
	"body",
	"div",
	"span",
	"p",
	
	"a",
	"img",
	"br",
	"b",
	"u",
	"strong",
	"em",
	"pre",
	
	"h1",
	"h2",
	"h3",
	"h4",
	"h5",
	
	"ul",
	"ol",
	"li",
	"dl",
	"dt",
	"dd",
	
	"table",
	"tbody",
	"tr",
	"td",
} ;

const std::set<std::string> white_list(
	Begin(white_list_array),
	End(white_list_array) ) ;

void OnStartElementNs(
    void *ctx,
    const xmlChar *name,
    const xmlChar **attr
)
{
	if ( white_list.find( std::string(reinterpret_cast<const char*>(name)) ) == white_list.end() )
		Log( "unknown element %1%", name ) ;
}
 
void OnEndElementNs(
    void* ctx,
    const xmlChar* name
)
{
}

} // end of local namespace

HtmlValidator::HtmlValidator( DataStream *out, const std::string& filename ) :
	m_( new Impl )
{
	assert( out != 0 ) ;
	m_->out = out ;

	xmlSAXHandler sax = {} ;
	sax.startElement	= &OnStartElementNs ;
	sax.endElement		= &OnEndElementNs ;
	
	Log( "this = %1%", this ) ;
		
	m_->ctx = ::htmlCreatePushParserCtxt(
		&sax, this, 0, 0,
		filename.c_str(),
		XML_CHAR_ENCODING_NONE ) ;
}

HtmlValidator::~HtmlValidator( )
{
}

std::size_t HtmlValidator::Read( char *data, std::size_t size )
{
	BOOST_THROW_EXCEPTION(
		Error() << expt::ErrMsg( "XmlFilter is not for reading!" )
	) ;
	
	// to shut off some warnings
	return 0 ;
}

std::size_t HtmlValidator::Write( const char *data, std::size_t size )
{
	Log( "read %1% bytes", size ) ;
	assert( m_->ctx != 0 ) ;
	
	int r = ::htmlParseChunk( m_->ctx, data, size, 0 ) ;
	if ( r != 0 )
		Log( "error %1%", r ) ;
	
	return m_->out->Write( data, size ) ;
}

void HtmlValidator::Finish()
{
	assert( m_->ctx != 0 ) ;
	
	int r = ::htmlParseChunk( m_->ctx, 0, 0, 1 ) ;
	if ( r != 0 )
		Log( "error %1%", r ) ;
}

} // end of namespace

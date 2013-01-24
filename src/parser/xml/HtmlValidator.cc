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

#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>

#include <cassert>

namespace wb {

struct HtmlValidator::Impl
{
	htmlParserCtxtPtr	ctx ;
	DataStream			*out ;
	xmlSAXHandler		sax ;
} ;

namespace {

void OnStartElementNs(
    void *ctx,
    const xmlChar *name,
    const xmlChar **attr
)
{
	Log( "read element %1%", name ) ;
}
 
void OnEndElementNs(
    void* ctx,
    const xmlChar* name
)
{
}

} // end of local namespace

HtmlValidator::HtmlValidator( DataStream *out ) :
	m_( new Impl )
{
	assert( out != 0 ) ;
	m_->out = out ;

	m_->ctx = 0 ;
	memset( &m_->sax, 0, sizeof(m_->sax) ) ;
	m_->sax.startElement	= &OnStartElementNs ;
	m_->sax.endElement		= &OnEndElementNs ; 
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
	if ( m_->ctx == 0 )
		m_->ctx = ::htmlCreatePushParserCtxt( &m_->sax, 0, data, size, "haha", XML_CHAR_ENCODING_NONE ) ;
	else
	{
		int r = ::htmlParseChunk( m_->ctx, data, size, 0 ) ;
		if ( r != 0 )
			Log( "error %1%", r ) ;
	}
	return m_->out->Write( data, size ) ;
}

void HtmlValidator::Finish()
{
	if ( m_->ctx != 0 )
	{
		int r = ::htmlParseChunk( m_->ctx, 0, 0, 1 ) ;
		if ( r != 0 )
			Log( "error %1%", r ) ;
		else
		{
			htmlDocPtr doc = m_->ctx->myDoc ;
			::htmlFreeParserCtxt( m_->ctx ) ;
			m_->ctx = 0 ;

			htmlDocPtr c = xmlCopyDoc( doc, 1 ) ;
			htmlDocDump( stdout, c ) ;
		}
	}
}

} // end of namespace

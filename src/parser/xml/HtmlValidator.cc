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
} ;

HtmlValidator::HtmlValidator( DataStream *out ) :
	m_( new Impl )
{
	assert( out != 0 ) ;
	m_->out = out ;

	m_->ctx = 0 ;
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
	if ( m_->ctx == 0 )
		m_->ctx = ::htmlCreatePushParserCtxt( 0, 0, data, size, "haha" ) ;
	else
		::htmlParseChunk( m_->ctx, data, size, 0 ) ;

	return m_->out->Write( data, size ) ;
}

void HtmlValidator::Finish()
{
	if ( m_->ctx != 0 )
	{
		::htmlParseChunk( m_->ctx, 0, 0, 1 ) ;

		::htmlDocDump( stderr, m_->ctx->myDoc ) ;
	}
}

} // end of namespace

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
#include "util/PrintF.hh"

#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>

#include <cassert>
#include <set>

#define THROW()				\
	BOOST_THROW_EXCEPTION(	\
			Error() << expt::ErrMsg( ::xmlCtxtGetLastError(m_->ctx)->message )	\
		) ;	

namespace wb {

namespace
{
	// hard code the HTML element white list for now
	const std::string white_list_array[] =
	{
		"html",	"a",	"img",
		"body",	"br",	"pre",
		"div",	"b",	"strong",
		"span",	"u",	"em",
		"p",
		
		"h1",	"ul",	"dd",	"table",
		"h2",	"ol",			"tbody",
		"h3",	"li",			"tr",
		"h4",	"dl",			"td",
		"h5",	"dt",
	} ;

	const std::set<std::string> white_list(
		Begin(white_list_array),
		End(white_list_array) ) ;

} // end of local namespace

struct HtmlValidator::Impl
{
	htmlParserCtxtPtr	ctx ;
	DataStream			*out ;
} ;

HtmlValidator::HtmlValidator( DataStream *out, const std::string& filename ) :
	m_( new Impl )
{
	assert( out != 0 ) ;
	m_->out = out ;

	::xmlSAXHandler sax = {} ;
	sax.startElement	= &HtmlValidator::OnStartElement ;
	sax.endElement		= &HtmlValidator::OnEndElement ;
	sax.characters		= &HtmlValidator::OnCharacters ;
	
	m_->ctx = ::htmlCreatePushParserCtxt(
		&sax, m_.get(), 0, 0,
		filename.c_str(),
		XML_CHAR_ENCODING_NONE ) ;
}

HtmlValidator::~HtmlValidator( )
{
}

HtmlValidator::Impl* HtmlValidator::GetImpl( void *pv )
{
	Impl *im = reinterpret_cast<Impl*>( pv ) ;
	assert( im != 0 ) ;
	assert( im->out != 0 ) ;
	return im ;
}

void HtmlValidator::OnStartElement(
	void				*pv,
	const unsigned char	*name,
	const unsigned char	**attr
)
{
	if ( white_list.find( std::string(reinterpret_cast<const char*>(name)) ) == white_list.end() )
		Log( "unknown element %1%", name ) ;

	Impl *im = GetImpl(pv) ;
	PrintF fmt(im->out) ;
	fmt( "<%1%>", name ) ;
}

void HtmlValidator::OnCharacters(
	void				*pv,
	const unsigned char	*chars,
	int					len
)
{
	Impl *im = GetImpl(pv) ;
	
	unsigned char out[1024] ;
	int out_len	= sizeof(out) ;
	
	const unsigned char *in = chars ;
	int in_len = len ;
	
	while ( true )
	{
		int iolen = in_len ;
		if ( ::htmlEncodeEntities( out, &out_len, in, &iolen, '\0' ) != 0 )
			break ;

		im->out->Write( reinterpret_cast<const char*>(out), out_len ) ;
		
		// finish!
		if ( iolen == in_len )
			break ;
		
		in		+= iolen ;
		in_len	-= iolen ;		
	}
}

void HtmlValidator::OnEndElement(
	void				*pv,
	const unsigned char	*name
)
{
	Impl *im = GetImpl(pv) ;
	PrintF fmt(im->out) ;
	fmt( "</%1%>", name ) ;
}

std::size_t HtmlValidator::Read( char *data, std::size_t size )
{
	// not supported
	assert( false ) ;
	
	// to shut off some warnings
	return 0 ;
}

std::size_t HtmlValidator::Write( const char *data, std::size_t size )
{
	assert( m_->ctx != 0 ) ;
	
	int r = ::htmlParseChunk( m_->ctx, data, size, 0 ) ;
	if ( r != 0 )
		THROW() ;
	
	return size ;
}

void HtmlValidator::Finish()
{
	assert( m_->ctx != 0 ) ;
	
	int r = ::htmlParseChunk( m_->ctx, 0, 0, 1 ) ;
	if ( r != 0 )
		THROW() ;
}

} // end of namespace

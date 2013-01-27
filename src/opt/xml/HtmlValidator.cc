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
#include "parser/StreamParser.hh"
#include "util/CArray.hh"
#include "util/PrintF.hh"
#include "util/StringStream.hh"

#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>

#include <cassert>
#include <set>
#include <iostream>

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

struct HtmlValidator::Impl
{
	htmlParserCtxtPtr	ctx ;
	DataStream			*out ;
	
	enum State
	{
		normal,
		unknown,
	} ;
	std::vector<State>	element ;
} ;

namespace
{
	int	DataStreamOutCallback(
		void		*user, 
		const char	*buffer, 
		int len )
	{
		DataStream *out = reinterpret_cast<DataStream*>(user) ;
		return static_cast<int>(out->Write( buffer, len )) ;
	}
	
	int WhateverCloseCallback( void * )
	{
		return 0 ;
	}
}

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
		XML_CHAR_ENCODING_UTF8 ) ;
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
	const unsigned char	*uname,
	const unsigned char	**attr
)
{
	const std::string name( reinterpret_cast<const char*>(uname) ) ;
	
	Impl *im = GetImpl(pv) ;
	if ( !im->element.empty() && im->element.back() == Impl::unknown )
	{
		Log( "element %1% inside unknown element, ignored", name ) ;
		im->element.push_back( Impl::unknown ) ;
		return ;
	}
	
	if ( white_list.find(name) == white_list.end() )
	{
		Log( "unknown element %1%", name ) ;
		im->element.push_back( Impl::unknown ) ;
		return ;
	}
	
	// don't write the root element tags because we only save the "inner-html"
	// the javascript will put it inside a <div>
	if ( ignore_list.find(name) == ignore_list.end() && !im->element.empty() )
	{
		PrintF fmt(im->out) ;
		fmt( "<%1%", name ) ;
		
		while ( attr != 0 && attr[0] != 0 && attr[1] != 0 )
		{
			fmt( " %1%=\"%2%\"", (char*)attr[0], (char*)attr[1] ) ;
			attr += 2 ;
		}
		
		fmt( ">" ) ;
	}
	
	im->element.push_back( Impl::normal ) ;
}

void HtmlValidator::OnCharacters(
	void				*pv,
	const unsigned char	*chars,
	int					len
)
{
	Impl *im = GetImpl(pv) ;
	if ( im->element.empty() || im->element.back() == Impl::unknown )
		return ;

	xmlOutputBufferPtr xobp = xmlOutputBufferCreateIO(
		&DataStreamOutCallback,
		&WhateverCloseCallback,
		im->out,
		0 ) ;

	std::vector<unsigned char> copy( chars, chars+len ) ;
	copy.push_back( '\0' ) ;
	xmlOutputBufferWriteEscape( xobp, &copy[0], 0 ) ;

	xmlOutputBufferClose( xobp ) ;
}

void HtmlValidator::OnEndElement(
	void				*pv,
	const unsigned char	*uname
)
{
	const std::string name( reinterpret_cast<const char*>(uname) ) ;

	Impl *im = GetImpl(pv) ;
	if ( ignore_list.find(name) == ignore_list.end() )
	{
		if ( !im->element.empty() && im->element.back() == Impl::normal )
		{
			PrintF fmt(im->out) ;
			fmt( "</%1%>", name ) ;
		}
	}
	if ( !im->element.empty() )
		im->element.pop_back() ;
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

/// Parsed according to W3C specification: http://www.w3.org/TR/html5/syntax.html
void HtmlValidator::Parse( DataStream *in, DataStream *out )
{
	StreamParser inp( in ) ;
	
	// throw away stuff until first open tag
	StreamParser::Result r = inp.ReadUntil( '<', DevNull() ) ;
	
	while ( r.found )
	{
		// throw away the '<'
		inp.Consume(1, DevNull() ) ;
	
		StringStream tag, attr ;
		r = inp.ReadUntilAny( " >", &tag ) ;
		
		// tag not closed yet. read its attributes
		if ( r.target == ' ' )
			inp.ReadUntil( '>', &attr ) ;
		
		bool good = CheckElement( tag.Str(), attr.Str() ) ;
		
		// if the element is in the white list, we consider it filtered in
		// and scan for next element
		if ( good )
		{
			PrintF fmt(out);
			fmt( "<%1% %2%", tag.Str(), attr.Str() ) ;
		}
		
		// element not in the white list. we will skip everything inside it
		// until its end tag
		else
		{
			inp.ReadUntil( "</" + tag.Str(), DevNull() ) ;
			inp.ReadUntil( '>', DevNull() ) ;
		}
		
		r = inp.ReadUntil( '<', out ) ;
	}
}

/// List of HTML5 tags can be omitted:
/// http://www.w3.org/TR/html5/syntax.html#syntax-tag-omission
bool HtmlValidator::CheckElement( const std::string& element, const std::string& attr )
{
	Log( "read tag %1% %2%", element, attr ) ;
	return white_list.find(element) != white_list.end() ;
}

} // end of namespace

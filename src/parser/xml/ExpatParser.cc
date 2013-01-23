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

#include "ExpatParser.hh"
#include "XmlHandler.hh"

#include <expat.h>

#include <cassert>

namespace wb {

struct ExpatParser::Expat
{
	XML_Parser	p;
} ;

ExpatParser::ExpatParser( XmlHandler *out ) :
	m_xpt( new Expat ),
	m_out( out )
{
	assert( out != 0 ) ;
	
	m_xpt->p = ::XML_ParserCreate( "UTF-8" ) ;
	
	if ( m_xpt->p == 0 )
		BOOST_THROW_EXCEPTION(
			Error() << expt::ErrMsg( "cannot create expat parser" )
		) ;
	
	::XML_SetElementHandler(
		m_xpt->p,
		&ExpatParser::StartElement,
		&ExpatParser::EndElement ) ;
	
	::XML_SetUserData( m_xpt->p, this ) ;
}

ExpatParser::~ExpatParser( )
{
	::XML_ParserFree( m_xpt->p ) ;
}

std::size_t ExpatParser::Read( char *data, std::size_t size )
{
	BOOST_THROW_EXCEPTION(
		Error() << expt::ErrMsg( "XmlFilter is not for reading!" )
	) ;
	
	// to shut off some warnings
	return 0 ;
}

std::size_t ExpatParser::Write( const char *data, std::size_t size )
{
	if ( ::XML_Parse( m_xpt->p, data, size, size == 0 ) == 0 )
		BOOST_THROW_EXCEPTION(
			Error() << expt::ErrMsg( ::XML_ErrorString(::XML_GetErrorCode( m_xpt->p ) ) )
		) ;
	return size ;
}

void ExpatParser::StartElement( void *vpthis, const char *element, const char **attr )
{
	assert( vpthis != 0 ) ;
	
	ExpatParser *pthis = reinterpret_cast<ExpatParser*>( vpthis ) ;
	assert( pthis->m_out != 0 ) ;
	
	pthis->m_out->StartElement( element, attr ) ;
}

void ExpatParser::EndElement( void *vpthis, const char *element )
{
	assert( vpthis != 0 ) ;
	
	ExpatParser *pthis = reinterpret_cast<ExpatParser*>( vpthis ) ;
	assert( pthis->m_out != 0 ) ;
	
	pthis->m_out->EndElement( element ) ;
}

} // end of namespace

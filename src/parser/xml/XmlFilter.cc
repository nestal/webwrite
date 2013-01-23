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

#include "XmlFilter.hh"

#include "util/DataStream.hh"
#include "log/Log.hh"

namespace wb {

XmlFilter::XmlFilter( DataStream *out ) :
	m_out( out )
{
}

void XmlFilter::StartElement( const char *element, const char **attr )
{
	Log( "find element %1%", element ) ;
}

void XmlFilter::EndElement( const char *element )
{
}

} // end of namespace

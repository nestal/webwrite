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

#include "Error.hh"

#include <libxml/xmlerror.h>

namespace xml { namespace expt {

void ThrowLastError( const char *func, const char *src_file, int src_line )
{
	xmlErrorPtr err = xmlGetLastError() ;
	if ( err != 0 )
		throw Error()
			
			// info thrown by BOOST_THROW_EXCEPTION
			<< boost::throw_function(func)
			<< boost::throw_file(src_file)
			<< boost::throw_line(src_line)
			
			// libxml2 error info
			<< Msg_( err->message )
			<< File_( err->file )
			<< Line_( err->line )
			<< Code_( err->code )
			<< Column_( err->int2 ) ;
}

} } // end of namespace

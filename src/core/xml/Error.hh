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

#pragma once

#include "util/Exception.hh"

namespace xml {

namespace expt
{
	struct Error : virtual wb::Exception
	{
	} ;
	
	typedef boost::error_info<struct Msg, 		std::string>	Msg_ ;
	typedef boost::error_info<struct File,		std::string>	File_ ;
	typedef boost::error_info<struct Line,		int>			Line_ ;
	typedef boost::error_info<struct Code,		int>			Code_ ;
	typedef boost::error_info<struct Column,	int>			Column_ ;
	
	void ThrowLastError( const char *func, const char *src_file, int src_line ) ;
}

#define THROW_LAST_XML_EXCEPTION()	\
	xml::expt::ThrowLastError( __PRETTY_FUNCTION__, __FILE__, __LINE__ )

} // end of namespace


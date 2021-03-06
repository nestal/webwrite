/*
	webwrite: an GPL wiki-like website with in-place editing
	Copyright (C) 2012  Wan Wai Ho

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#pragma once

#include "util/Exception.hh"
#include "util/FileSystem.hh"
#include <string>
#include <boost/function.hpp>

namespace wb {

class Source ;
class File ;

/*!	\brief	HTTP multi-part form data. Defined in RFC2388/1867.
*/
class FormData
{
public :
	class Error : public Exception {} ;
	
	typedef boost::error_info<struct ExpB, std::string>	ExpectedBoundary ;
	typedef boost::error_info<struct ActB, std::string>	ActualBoundary ;

	typedef boost::function<
		void ( const fs::path& path, const std::string& filename, File& file, const std::string& mime )
	> Callback ;

public :
	FormData( Source *in, const std::string& ctype ) ;

	void Save( const fs::path& path, const Callback& callback ) ;

private :
	Source		*m_in ;
	std::string	m_boundary ;
} ;

} // end of namespace

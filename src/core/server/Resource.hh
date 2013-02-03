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

#include "util/FileSystem.hh"
#include "util/Exception.hh"

namespace wb {

class Resource
{
public :
	struct Error : virtual public Exception {} ;

	Resource() ;
	explicit Resource( const std::string& uri ) ;
	
	/// a path to the resource relative to the "wb-root"
	const fs::path& Path() const ;

	fs::path ReDirPath() const ;
	fs::path UrlPath() const ;
	fs::path DataPath() const ;
	fs::path AtticPath() const ;
	
	/// The "filename" in the URL (i.e. the string after the last slash)
	std::string Filename() const ;
	
	std::string ParentName() const ;

	std::string Name() const ;
	
	std::string Type() const ;

	static std::string DecodeName( const std::string& uri ) ;

private :
	fs::path		m_path ;
} ;

} // end of namespace

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

#include <ctime>
#include <memory>

namespace wb {

class Json ;

class Resource
{
public :
	struct Error : virtual public Exception {} ;
	typedef boost::error_info<struct InvalidUri, std::string>	InvalidUri_ ;

	struct Meta
	{
		std::string		name ;
		std::string		type ;
		std::time_t		modified ;
	} ;

public :
	Resource() ;
	explicit Resource( const std::string& uri ) ;
	~Resource() ;

	bool CheckRedir( const std::string& uri ) const ;

	/// a path to the resource relative to the "wb-root"
	const fs::path& Path() const ;

	fs::path ReDirPath() const ;
	fs::path UrlPath() const ;
	fs::path DataPath() const ;
	fs::path AtticPath() const ;
	fs::path MetaPath() const ;
	
	/// The "filename" in the URL (i.e. the string after the last slash)
	std::string Filename() const ;
	
	std::string ParentName() const ;

	std::string Name() const ;
	
	std::string Type() const ;

	Meta GetMeta() const ;
	void SaveMeta() const ;
	
	void MoveToAttic() const ;

	static std::string DecodeName( const std::string& uri ) ;

private :
	template <typename CharMapT>
	static std::string DecodePercent( const std::string& uri, CharMapT cmap ) ;

	void LoadMeta() const ;

	std::string DeduceType() const ;
	std::string DeduceName() const ;

private :
	fs::path					m_path ;
	mutable std::auto_ptr<Meta>	m_meta ;
} ;

} // end of namespace

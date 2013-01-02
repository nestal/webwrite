/*
	grive: an GPL program to sync a local directory with Google Drive
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

#include "DataStream.hh"
#include "Exception.hh"
#include "FileSystem.hh"
#include "Types.hh"

#include <string>

#ifdef WIN32
	#include <memory>
#endif

namespace wb {

/*!	\brief	a wrapper class for file read/write.

	It will throw exceptions when encountered errors.
*/
class File : public DataStream
{
public :
	struct Error : virtual Exception {} ;

public :
	File() ;
	File( const fs::path& path ) ;
	File( const fs::path& path, int mode ) ;
	~File( ) ;

	void OpenForRead( const fs::path& path ) ;
	void OpenForWrite( const fs::path& path, int mode = 0600 ) ;
	void Close() ;
	bool IsOpened() const ;
	
	std::size_t Read( char *ptr, std::size_t size ) ;
	std::string ReadLine( std::size_t max ) ;
	std::size_t Write( const char *ptr, std::size_t size ) ;

	off_t Seek( off_t offset, int whence ) ;
	off_t Tell() const ;
	u64_t Size() const ;
	
	void Chmod( int mode ) ;

	void* Map( off_t offset, std::size_t length ) ;
	static void UnMap( void *addr, std::size_t length ) ;
	
private :
	void Open( const fs::path& path, int flags, int mode ) ;
	
private :
	int	m_fd ;
} ;
	
} // end of namespace

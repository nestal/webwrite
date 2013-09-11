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

#include "File.hh"

#include "debug/Assert.hh"

// boost headers
#include <boost/throw_exception.hpp>
#include <boost/exception/errinfo_api_function.hpp>
#include <boost/exception/errinfo_errno.hpp>
#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/errinfo_file_open_mode.hpp>
#include <boost/exception/info.hpp>

#include <climits>
#include <cstdlib>
#include <cstring>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#ifdef WIN32
	#include <io.h>
	typedef int ssize_t ;
#else
	#include <sys/mman.h>
#endif

// local functions
namespace {

off_t LSeek( int fd, off_t offset, int whence )
{
	WB_ASSERT( fd >= 0 ) ;
	
	off_t r = ::lseek( fd, offset, whence ) ;
	if ( r == static_cast<off_t>(-1) )
	{
		BOOST_THROW_EXCEPTION(
			wb::File::Error()
				<< boost::errinfo_api_function("lseek")
				<< boost::errinfo_errno(errno)
		) ;
	}
	
	return r ;
}

struct stat FStat( int fd )
{
	struct stat s = {} ;
	if ( ::fstat( fd, &s ) != 0 )
	{
		BOOST_THROW_EXCEPTION(
			wb::File::Error()
				<< boost::errinfo_api_function("fstat")
				<< boost::errinfo_errno(errno)
		) ;
	}
	return s ;
}

} // end of local functions

namespace wb {

File::File( ) : m_fd( -1 )
{
}

/**	Opens the file for reading.
	\param	path	Path to the file to be opened.
	\throw	Error	When the file cannot be openned.
*/
File::File( const fs::path& path ) : m_fd( -1 )
{
	OpenForRead( path ) ;
}

/**	Opens the file for writing.
	\param	path	Path to the file to be opened.
	\param	mode	Mode of the file to be created, e.g. 0600 for user
					readable/writable.
	\throw	Error	When the file cannot be opened.
*/
File::File( const fs::path& path, int mode ) : m_fd( -1 )
{
	OpenForWrite( path, mode ) ;
}

/**	The destructor will close the file. 
*/
File::~File( )
{
	Close() ;
}

void File::Open( const fs::path& path, int flags, int mode )
{
	if ( IsOpened() )
		Close() ;
	
	WB_ASSERT( m_fd == -1 ) ;
	m_fd = ::open( path.string().c_str(), flags, mode ) ;
	if ( m_fd == -1 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("open")
				<< boost::errinfo_errno(errno)
				<< boost::errinfo_file_name(path.string())
		) ;
	}
}

void File::OpenForRead( const fs::path& path )
{
	int flags = O_RDONLY ;
#ifdef WIN32
	flags |= O_BINARY ;
#endif
	Open( path, flags, 0 ) ;
}

void File::OpenForWrite( const fs::path& path, int mode )
{
	int flags = O_CREAT|O_RDWR|O_TRUNC ;
#ifdef WIN32
	flags |= O_BINARY ;
#endif
	Open( path, flags, mode ) ;
}

fs::path File::OpenRandom( const fs::path& tmplate )
{
#ifdef WIN32
	BOOST_THROW_EXCEPTION( Error() ) ;
#else
	if ( IsOpened() )
		Close() ;

	char rand_file[PATH_MAX] ;
	std::string tmpl_str = tmplate.string() + "XXXXXX" ;
	::strncpy( rand_file, tmpl_str.c_str(), sizeof(rand_file) ) ;

	m_fd = ::mkstemp(rand_file) ;
	if ( m_fd == -1 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("mkstemp")
				<< boost::errinfo_errno(errno)
				<< boost::errinfo_file_name(tmplate.string())
		) ;
	}
	
	return fs::path(rand_file) ;
#endif
}

void File::Close()
{
	if ( IsOpened() )
	{
		::close( m_fd ) ;
		m_fd = -1 ;
	}
}

bool File::IsOpened() const
{
	return m_fd != -1 ;
}

/**	Read bytes from file. See DataStream::Read() for details.
	\throw	Error	In case of any error.
*/
std::streamsize File::read( char *ptr, std::streamsize size )
{
	WB_ASSERT( IsOpened() ) ;
	ssize_t count = ::read( m_fd, ptr, size ) ;
	if ( count == -1 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("read")
				<< boost::errinfo_errno(errno)
		) ;
	}
	return count ;
}

std::streamsize File::write( const char *ptr, std::streamsize size )
{
	WB_ASSERT( IsOpened() ) ;
	ssize_t count = ::write( m_fd, ptr, size ) ;
	if ( count == -1 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("write")
				<< boost::errinfo_errno(errno)
		) ;
	}
	return count ;
}

off_t File::Seek( off_t offset, int whence )
{
	WB_ASSERT( IsOpened() ) ;
	return LSeek( m_fd, offset, whence ) ;
}

off_t File::Tell() const
{
	WB_ASSERT( IsOpened() ) ;
	return LSeek( m_fd, 0, SEEK_CUR ) ;
}

u64_t File::Size() const
{
	WB_ASSERT( IsOpened() ) ;
	
	struct stat s = FStat(m_fd) ;
	
	WB_ASSERT( s.st_size >= 0 ) ;
	return static_cast<uint64_t>( s.st_size ) ;
}

void File::Chmod( int mode )
{
	WB_ASSERT( IsOpened() ) ;
#ifndef WIN32	
	if ( ::fchmod( m_fd, mode ) != 0 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("fchmod")
				<< boost::errinfo_errno(errno)
		) ;
	}
#endif
}

/// This function is not implemented in win32 yet.
void* File::Map( off_t offset, std::size_t length )
{
	WB_ASSERT( IsOpened() ) ;
	
#ifdef WIN32
	WB_ASSERT( false ) ;
	return 0 ;
#else
	void *addr = ::mmap( 0, length, PROT_READ, MAP_PRIVATE, m_fd, offset ) ;
	if ( addr == reinterpret_cast<void*>( -1 ) )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("mmap")
				<< boost::errinfo_errno(errno)
		) ;
	}
	return addr ;
#endif
}

void File::UnMap( void *addr, std::size_t length )
{
#ifndef WIN32
	if ( ::munmap( addr, length ) != 0 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("munmap")
				<< boost::errinfo_errno(errno)
		) ;
	}
#endif
}

struct stat File::Stat() const
{
	struct stat result = {} ;
	if ( ::fstat( m_fd, &result ) != 0 )
	{
		BOOST_THROW_EXCEPTION(
			Error()
				<< boost::errinfo_api_function("fstat")
				<< boost::errinfo_errno(errno)
		) ;
	}
	return result ;
}

} // end of namespace

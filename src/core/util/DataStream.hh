/*
	webwrite: an GPL program to sync a local directory with Google Drive
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

#include <boost/iostreams/concepts.hpp>
#include <cstddef>

namespace wb {

namespace io = boost::iostreams ;

/**	\brief	Encapsulation of data streams. Useful for unit tests.
	This class provides two functions: Read() and Write().
*/
class Source : public boost::iostreams::source
{
protected :
	virtual ~Source() {}
	
public :
	/**	Reading from the stream. The caller indicates that it wants
		to read `size` bytes and must provide enough space pointed
		by `data`.
		\param	data	Buffer to hold the data read from the stream
						Must have at least `size` bytes.
		\param	size	Number of bytes the caller wants to read.
		\throw	wb::Exception	In case of any error.
		\return			The number of byte actually read from the stream.
						0 indicates the end of stream, i.e. you will
						still get 0 if you call again.
	*/
	virtual std::streamsize read( char *data, std::streamsize size ) = 0 ;
} ;

class Sink : public boost::iostreams::sink
{
protected :
	virtual ~Sink() {}

public :
	virtual std::streamsize write( const char *data, std::streamsize size ) = 0 ;
} ;

/// Stream for /dev/null, i.e. read and writing nothing
Sink* DevNull() ;

class RealSink : public Sink
{
public :
	explicit RealSink( Sink *out ) ;
	
	std::streamsize write( const char *data, std::streamsize size ) ;

private :
	Sink	*m_out ;
} ;

} // end of namespace

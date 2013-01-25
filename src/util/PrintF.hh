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

#include <boost/format.hpp>
#include <string>

namespace wb {

class DataStream ;

class PrintF
{
public :
	explicit PrintF( DataStream *out ) ;
	
	std::size_t operator()( const std::string& fmt ) ;
	
	// template version of printf
	template <typename P1>
	std::size_t operator()( const std::string& fmt, const P1& p1 ) ;

	template <typename P1, typename P2>
	std::size_t operator()( const std::string& fmt, const P1& p1, const P2& p2 ) ;

	template <typename P1, typename P2, typename P3>
	std::size_t operator()(
		const std::string& fmt,
		const P1& p1, const P2& p2, const P3& p3 ) ;

	template <typename P1, typename P2, typename P3, typename P4>
	std::size_t operator()(
		const std::string& fmt,
		const P1& p1, const P2& p2,
		const P3& p3, const P4& p4 ) ;

private :
	std::size_t PutS( const char *str, std::size_t size ) ;

private :
	DataStream	*m_out ;
} ;

template <typename P1>
std::size_t PrintF::operator()( const std::string& fmt, const P1& p1 )
{
	return operator()((boost::format(fmt) % p1).str());
}

template <typename P1, typename P2>
std::size_t PrintF::operator()( const std::string& fmt, const P1& p1, const P2& p2 )
{
	return operator()((boost::format(fmt) % p1 % p2).str()) ;
}

template <typename P1, typename P2, typename P3>
std::size_t PrintF::operator()( const std::string& fmt, const P1& p1, const P2& p2, const P3& p3 )
{
	return operator()((boost::format(fmt) % p1 % p2 % p3).str()) ;
}

template <typename P1, typename P2, typename P3, typename P4>
std::size_t PrintF::operator()(
	const std::string& fmt,
	const P1& p1,
	const P2& p2,
	const P3& p3,
	const P4& p4 )
{
	return operator()((boost::format(fmt) % p1 % p2 % p3 % p4).str()) ;
}

} // end of namespace

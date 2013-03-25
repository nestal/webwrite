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

#include "parser/Json.hh"
#include <boost/test/unit_test.hpp>

#include <iostream>

using namespace wb ;

namespace
{
	struct F
	{
	} ;
	
	struct Int
	{
		int me ;
		Int(int i):me(i)
		{
		}
	} ;

	struct Map ;
	struct Array ;

	struct Base
	{
		int me ;
		Base *parent ;
		Base( int i, Base *p = 0 ) : me(i), parent(p)
		{
		}

		void Ident()
		{
			std::cout << '\t' ;
			if ( parent != 0 )
				parent->Ident() ;
		}

		Base( const Base& m ) : me(m.me), parent(m.parent)
		{
		}

		virtual Base& End() = 0 ;

		virtual Base& Value( const Int& i ) = 0 ;
		virtual Map& Key( const std::string& key )
		{
			throw -1 ;
		}
		
		Map AddMap(int id) ;
		Array AddArray( int id );
	
	protected :
		~Base()
		{
		}
	} ;
	
	Base& Base::End()
	{
		return parent == 0 ? *this : *parent ;
	}

	struct Map : public Base
	{
		Map( int i, Base *p = 0 ) : Base(i,p)
		{
			Ident();
			std::cout << "start map " << me << std::endl ;
		}
		Map& Value( const Int& i ) ;
		Map& Key( const std::string& k )
		{
			Ident();
			std::cout << me << "map[" << k << "]" ;
			return *this ;
		}

		Base& End()
		{
			Ident();
			std::cout << "end map " << me << std::endl ;

			return Base::End() ;
		}
	} ;

	struct Array : public Base
	{
		Array( int i, Base *p = 0 ) : Base(i,p)
		{
			Ident();
			std::cout << "start array " << me << std::endl ;
		}
		Array& Value( const Int& i ) ;
		Base& End()
		{
			Ident();
			std::cout << "end array " << me << std::endl ;

			return Base::End() ;
		}
	} ;

	Map Base::AddMap(int id)
	{
		Ident();
		std::cout << me << " add map " << id << std::endl ;
		return Map(id,this) ;
	}
	
	Array Base::AddArray(int id)
	{
		Ident();
		std::cout << me << " add array " << id << std::endl ;
		return Array(id,this) ;
	}

	Map& Map::Value( const Int& i )
	{
		std::cout << " = " << i.me << std::endl ;
		return *this ;
	}

	Array& Array::Value( const Int& i )
	{
		Ident();
		std::cout << me << " array add " << i.me << std::endl ;
		return *this ;
	}
}

BOOST_FIXTURE_TEST_SUITE( JsonTest, F )

BOOST_AUTO_TEST_CASE( Test )
{
	Json a( "abc" ) ;
	BOOST_CHECK_EQUAL( a.Str(), "abc" ) ;
}

BOOST_AUTO_TEST_CASE( Try )
{
	Map(1).
		Key("2").
		Value( Int(2) ).
		Key("3").
		Value( Int(3) ).
		AddMap(4).
			Key("5").
			Value(Int(6)).
		End().
		Key("7").
		Value( Int(7) ).
		AddArray(8).
			Value(Int(9)).
			Value(Int(10)).
			AddMap(11).
				Key("12").Value(12).
				Key("13").Value(13).
			End().
			Value(Int(14)).
		End().
	End() ;
}

BOOST_AUTO_TEST_SUITE_END()

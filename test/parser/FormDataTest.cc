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

#include "parser/FormData.hh"
#include "util/File.hh"

#include <boost/test/unit_test.hpp>
#include <boost/exception/diagnostic_information.hpp>

using namespace wb ;

void DummyCallback(
		const fs::path&		path,
		const std::string&	filename,
		File&				file,
		const std::string&	mime )
{
}

BOOST_AUTO_TEST_SUITE( FormDataTest )

BOOST_AUTO_TEST_CASE( Test )
{
	File form_data( TEST_DATA "test.pdf.form" ) ;
	FormData subject( &form_data, "multipart/form-data; boundary="
		"---------------------------101378736882613805899358823" ) ;
	subject.Save( "./", DummyCallback ) ;
	
	File form_data2( TEST_DATA "textures.form" ) ;
	FormData subject2( &form_data2, "multipart/form-data; boundary="
		"---------------------------11351845291583120309948566114" ) ;
	subject2.Save( "./", DummyCallback ) ;
}

BOOST_AUTO_TEST_SUITE_END()

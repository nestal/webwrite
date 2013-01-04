#
#	webwrite: an GPL wiki-like website with in-place editing
#	Copyright (C) 2012  Wan Wai Ho

#	This program is free software; you can redistribute it and/or
#	modify it under the terms of the GNU General Public License
#	as published by the Free Software Foundation version 2
#	of the License.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program; if not, write to the Free Software
#	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

project( webwrite )
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(Boost_USE_STATIC_LIBS 1)

find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(JSONC REQUIRED)
find_package(FCGI REQUIRED)
find_package(CppUnit)
find_package(BFD)

include_directories(
	${JSONC_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${FCGI_INCLUDE_DIR}
	${webwrite_SOURCE_DIR}/src
	${webwrite_SOURCE_DIR}/test
)

# build bfd classes if libbfd is found
if ( BFD_FOUND )
        set( OPT_LIBS   ${DL_LIBRARY} ${BFD_LIBRARY} )
        file( GLOB OPT_SRC
                ${webwrite_SOURCE_DIR}/src/bfd/*.cc
        )
        add_definitions( -DHAVE_BFD )
        
endif ( BFD_FOUND )

add_definitions(
	-DTEST_DATA="${webwrite_SOURCE_DIR}/test/data/"
	-DSRC_DIR="${webwrite_SOURCE_DIR}/src"
)

file (GLOB WEBWRITE_LIB_SRC
	${OPT_SRC}
    ${webwrite_SOURCE_DIR}/src/util/*.hh
    ${webwrite_SOURCE_DIR}/src/util/*.cc
    ${webwrite_SOURCE_DIR}/src/parser/*.cc
)

file (GLOB WEBWRITE_SRC
	${WEBWRITE_LIB_SRC}
    ${webwrite_SOURCE_DIR}/src/*.hh
    ${webwrite_SOURCE_DIR}/src/*.cc
)

add_executable( webwrite
	${WEBWRITE_SRC}
)

target_link_libraries( webwrite
	${OPT_LIBS}
	${Boost_LIBRARIES}
	${JSONC_LIBRARY}
	${FCGI_LIBRARY}
)

install(TARGETS webwrite RUNTIME DESTINATION bin)
#install(FILES doc/grive.1 DESTINATION share/man/man1 )

IF ( CPPUNIT_FOUND )
	message("-- Building unit tests")
	include_directories( ${CPPUNIT_INCLUDE_DIR} )
    
    # list of test source files here
	file(GLOB TEST_SRC
		test/*.cc
		test/parser/*.cc
		src/parser/*.cc
	)

	add_executable( unittest
		test/UnitTest.cc
		${TEST_SRC}
		${WEBWRITE_LIB_SRC}
	)

	target_link_libraries( unittest
		${CPPUNIT_LIBRARY}
		${JSONC_LIBRARY}
		${Boost_LIBRARIES}
		${OPT_LIBS}
	)
        
ENDIF ( CPPUNIT_FOUND )

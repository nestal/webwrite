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
set( WEBWRITE_VERSION "0.1" )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(Boost_USE_MULTITHREADED ON )

if ( WIN32 )
	# don't use shared library in win32
	set(Boost_USE_STATIC_LIBS 	1)

	# disable msvc stupid warnings
	add_definitions( -D_SCL_SECURE_NO_WARNINGS )
endif ( WIN32 )

find_package(Boost COMPONENTS filesystem system regex unit_test_framework REQUIRED)
find_package(JSONC REQUIRED)
find_package(FCGI REQUIRED)
find_package(CppUnit)
find_package(BFD)
find_package(LibXml2)

if ( WIN32 )
	find_package(iconv)
endif ( WIN32 )

include_directories(
	${JSONC_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${FCGI_INCLUDE_DIR}
	${webwrite_SOURCE_DIR}/src
	${webwrite_SOURCE_DIR}/test
)

add_definitions(
	-DTEST_DATA="${webwrite_SOURCE_DIR}/test/data/"
	-DSRC_DIR="${webwrite_SOURCE_DIR}/src"
	-DVERSION="${WEBWRITE_VERSION}"
)

# build bfd classes if libbfd is found
if ( BFD_FOUND )
	set( OPT_LIBS   ${DL_LIBRARY} ${BFD_LIBRARY} )
	file( GLOB BFD_SRC
		${webwrite_SOURCE_DIR}/src/bfd/*.cc
	)
	add_definitions( -DHAVE_BFD )
	set(OPT_SRC ${BFD_SRC} )
        
endif ( BFD_FOUND )

if ( LIBXML2_FOUND )
	set( OPT_LIBS	${LIBXML2_LIBRARIES} ${ICONV_LIBRARIES} ${OPT_LIBS} )
	include_directories( ${LIBXML2_INCLUDE_DIR} ${ICONV_INCLUDE_DIRS} )
	
	file( GLOB XML_SRC
		${webwrite_SOURCE_DIR}/src/parser/xml/*.cc
	)
	set(OPT_SRC ${XML_SRC} ${OPT_SRC})

	add_definitions( -DHAVE_LIBXML2 ${LIBXML2_DEFINITIONS} )
endif ( LIBXML2_FOUND )


file (GLOB WEBWRITE_LIB_SRC
	${OPT_SRC}
    ${webwrite_SOURCE_DIR}/src/log/*.hh
    ${webwrite_SOURCE_DIR}/src/log/*.cc
    ${webwrite_SOURCE_DIR}/src/parser/*.hh
    ${webwrite_SOURCE_DIR}/src/parser/*.cc
    ${webwrite_SOURCE_DIR}/src/util/*.hh
    ${webwrite_SOURCE_DIR}/src/util/*.cc
    ${webwrite_SOURCE_DIR}/src/server/*.hh
    ${webwrite_SOURCE_DIR}/src/server/*.cc
)

add_library( wb_core
	${WEBWRITE_LIB_SRC}
)

file (GLOB WEBWRITE_SRC
	${webwrite_SOURCE_DIR}/src/*.hh
    ${webwrite_SOURCE_DIR}/src/*.cc
)

add_executable( webwrite
	${WEBWRITE_SRC}
)

target_link_libraries( webwrite
	wb_core
	${OPT_LIBS}
	${Boost_LIBRARIES}
	${JSONC_LIBRARY}
	${FCGI_LIBRARY}
)

install(TARGETS webwrite RUNTIME DESTINATION bin)

if ( CPPUNIT_FOUND )
	message("-- Building unit tests")
	include_directories( ${CPPUNIT_INCLUDE_DIR} )
    
    # list of test source files here
	file(GLOB TEST_SRC
		test/*.cc
		test/parser/*.cc
	)

	add_executable( unittest
		test/UnitTest.cc
		${TEST_SRC}
		${WEBWRITE_LIB_SRC}
	)

	target_link_libraries( unittest
		wb_core
		${CPPUNIT_LIBRARY}
		${JSONC_LIBRARY}
		${Boost_LIBRARIES}
		${OPT_LIBS}
	)

endif ( CPPUNIT_FOUND )

add_executable( btest test/parser/HtmlValidatorTest.cc )
target_link_libraries( btest wb_core
	${JSONC_LIBRARY}
	${Boost_LIBRARIES}
	${OPT_LIBS}
)
set_target_properties( btest PROPERTIES COMPILE_FLAGS -DBOOST_TEST_DYN_LINK ) 
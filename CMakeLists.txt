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

find_package(Boost COMPONENTS filesystem system regex unit_test_framework REQUIRED)
find_package(JSONC REQUIRED)
find_package(FCGI REQUIRED)
find_package(BFD)
find_package(LibXml2)

if ( WIN32 )
	find_package(iconv)
endif ( WIN32 )

set(Boost_USE_MULTITHREADED ON )

if ( WIN32 )
	# don't use shared library in win32
	set(Boost_USE_STATIC_LIBS 	1)

	# disable msvc stupid warnings
	add_definitions( -D_SCL_SECURE_NO_WARNINGS )
endif ( WIN32 )

include_directories(
	${JSONC_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${FCGI_INCLUDE_DIR}
	${webwrite_SOURCE_DIR}/src/core
	${webwrite_SOURCE_DIR}/src/opt
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
	file( GLOB_RECURSE OPT_SRC
		${webwrite_SOURCE_DIR}/src/opt/bfd/*.cc
		${webwrite_SOURCE_DIR}/src/opt/bfd/*.hh
	)
	add_definitions( -DHAVE_BFD )
endif ( BFD_FOUND )

if ( LIBXML2_FOUND )
	set( OPT_LIBS	${LIBXML2_LIBRARIES} ${ICONV_LIBRARIES} ${OPT_LIBS} )
	include_directories( ${LIBXML2_INCLUDE_DIR} ${ICONV_INCLUDE_DIRS} )
	
	file( GLOB_RECURSE XML_SRC
		${webwrite_SOURCE_DIR}/src/opt/xml/*.cc
		${webwrite_SOURCE_DIR}/src/opt/xml/*.hh
	)
	set(OPT_SRC ${XML_SRC} ${OPT_SRC})

	add_definitions( -DHAVE_LIBXML2 ${LIBXML2_DEFINITIONS} )
endif ( LIBXML2_FOUND )


file (GLOB_RECURSE WEBWRITE_CORE_SRC
    ${webwrite_SOURCE_DIR}/src/core/*.hh
    ${webwrite_SOURCE_DIR}/src/core/*.cc
)

add_library( wb_core
	${WEBWRITE_CORE_SRC}
	${OPT_SRC}
)

# not recursive for source in the root source dir
file (GLOB WEBWRITE_MAIN_SRC
    ${webwrite_SOURCE_DIR}/src/*.hh
    ${webwrite_SOURCE_DIR}/src/*.cc
)

add_executable( webwrite ${WEBWRITE_MAIN_SRC} )

target_link_libraries( webwrite
	wb_core
	${OPT_LIBS}
	${Boost_LIBRARIES}
	${JSONC_LIBRARY}
	${FCGI_LIBRARY}
)

install(TARGETS webwrite RUNTIME DESTINATION bin)

file (GLOB_RECURSE WEBWRITE_UT_SRC
#    ${webwrite_SOURCE_DIR}/test/*.hh
    ${webwrite_SOURCE_DIR}/test/*.cc
)

add_executable( btest
	${WEBWRITE_UT_SRC} )
	
target_link_libraries( btest wb_core
	${JSONC_LIBRARY}
	${Boost_LIBRARIES}
	${OPT_LIBS}
)
set_target_properties( btest PROPERTIES COMPILE_FLAGS -DBOOST_TEST_DYN_LINK ) 

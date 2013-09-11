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

if ( WIN32 )
	# don't use shared library in win32
	# we need this before find_package
	set(Boost_USE_STATIC_LIBS 	ON)

	# disable msvc stupid warnings
	add_definitions( -D_SCL_SECURE_NO_WARNINGS )

endif ( WIN32 )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# need these before find package
set(Boost_USE_MULTITHREADED ON )
set(Boost_DETAILED_FAILURE_MSG ON )

find_package(Boost COMPONENTS filesystem system regex date_time thread chrono timer unit_test_framework REQUIRED )
find_package(JSONC REQUIRED)
find_package(FCGI REQUIRED)
find_package(LibXML2 REQUIRED)

include_directories(
	${JSONC_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${FCGI_INCLUDE_DIR}
	${LibXML2_INCLUDE_DIR}
	${webwrite_SOURCE_DIR}/src/core
	${webwrite_SOURCE_DIR}/test
)

add_definitions(
	-DTEST_DATA="${webwrite_SOURCE_DIR}/test/data/"
	-DSRC_DIR="${webwrite_SOURCE_DIR}/src"
	-DVERSION="${WEBWRITE_VERSION}"
)

file (GLOB_RECURSE WEBWRITE_CORE_SRC
    ${webwrite_SOURCE_DIR}/src/core/*.hh
    ${webwrite_SOURCE_DIR}/src/core/*.cc
    ${webwrite_SOURCE_DIR}/src/core/crypto/*.h
    ${webwrite_SOURCE_DIR}/src/core/crypto/*.c
)

add_library( wb_core
	${WEBWRITE_CORE_SRC}
)

# not recursive for source in the root source dir
file (GLOB WEBWRITE_MAIN_SRC
    ${webwrite_SOURCE_DIR}/src/*.hh
    ${webwrite_SOURCE_DIR}/src/*.cc
)

add_executable( webwrite ${WEBWRITE_MAIN_SRC} )

target_link_libraries( webwrite
	wb_core
	${Boost_LIBRARIES}
	${JSONC_LIBRARY}
	${FCGI_LIBRARY}
	${LibXML2_LIBRARIES}
)

install(TARGETS webwrite RUNTIME DESTINATION bin)

# unit tests
file (GLOB_RECURSE WEBWRITE_UT_SRC
	${webwrite_SOURCE_DIR}/test/*.cc
)

add_executable( unittest
	${WEBWRITE_UT_SRC} )

target_link_libraries( unittest wb_core
	${Boost_LIBRARIES}
	${JSONC_LIBRARY}
	${LibXML2_LIBRARIES}
)

if ( WIN32 )
else ( WIN32 )
	set_target_properties( unittest
		PROPERTIES COMPILE_FLAGS	 -DBOOST_TEST_DYN_LINK )
endif (WIN32)
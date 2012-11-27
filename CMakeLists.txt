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

find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(JSONC REQUIRED)
find_package(BFD)

include_directories(
	${BOOST_INCLUDE_DIR}
	${JSONC_INCLUDE_DIR}
	${webwrite_SOURCE_DIR}/src
)

file (GLOB WEBWRITE_SRC
    ${webwrite_SOURCE_DIR}/src/*.cc
    ${webwrite_SOURCE_DIR}/src/util/*.cc
    ${webwrite_SOURCE_DIR}/src/parser/*.cc
)

# build bfd classes if libbfd is found
if ( BFD_FOUND )
        set( OPT_LIBS   ${DL_LIBRARY} ${BFD_LIBRARY} )
        file( GLOB OPT_SRC
                ${webwrite_SOURCE_DIR}/src/bfd/*.cc
        )
        add_definitions( -DHAVE_BFD )
        
endif ( BFD_FOUND )

add_executable( webwrite
	${OPT_SRC}
	${WEBWRITE_SRC}
)

target_link_libraries( webwrite
	fcgi
	${OPT_LIBS}
	${Boost_LIBRARIES}
	${JSONC_LIBRARY}
)

install(TARGETS webwrite RUNTIME DESTINATION bin)
#install(FILES doc/grive.1 DESTINATION share/man/man1 )

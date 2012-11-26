project( webwrite )

find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(
	${grive_SOURCE_DIR}/include
)

file (GLOB WEBWRITE_SRC
    ${webwrite_SOURCE_DIR}/*.cc
)

add_executable( webwrite
	${WEBWRITE_SRC}
)

target_link_libraries( webwrite
	fcgi
)

install(TARGETS webwrite RUNTIME DESTINATION bin)
#install(FILES doc/grive.1 DESTINATION share/man/man1 )

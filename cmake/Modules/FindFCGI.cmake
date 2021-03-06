# - Find FCGI
# This module finds an installed FCGI package.
#
# It sets the following variables:
#  FCGI_FOUND       - Set to false, or undefined, if JSON-C isn't found.
#  FCGI_INCLUDE_DIR - The JSON-C include directory.
#  FCGI_LIBRARY     - The JSON-C library to link against.

FIND_PATH(		FCGI_INCLUDE_DIR		NAMES fcgiapp.h		PATH_SUFFIXES include )
FIND_LIBRARY(	FCGI_LIBRARY_DEBUG		NAMES fcgi libfcgi	PATH_SUFFIXES Debug )
FIND_LIBRARY(	FCGI_LIBRARY_RELEASE	NAMES fcgi libfcgi	PATH_SUFFIXES Release )

SET( FCGI_LIBRARY
	debug		${FCGI_LIBRARY_DEBUG}
	optimized	${FCGI_LIBRARY_RELEASE}
)

IF (FCGI_INCLUDE_DIR AND FCGI_LIBRARY)
   SET(FCGI_FOUND TRUE)
ENDIF (FCGI_INCLUDE_DIR AND FCGI_LIBRARY)

IF (FCGI_FOUND)

   # show which JSON-C was found only if not quiet
   IF (NOT FCGI_FIND_QUIETLY)
      MESSAGE(STATUS "Found libfcgi: ${FCGI_LIBRARY}")
   ENDIF (NOT FCGI_FIND_QUIETLY)

ELSE (FCGI_FOUND)

   # fatal error if JSON-C is required but not found
   IF (FCGI_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find libfcgi")
   ENDIF (FCGI_FIND_REQUIRED)

ENDIF (FCGI_FOUND)

cmake_minimum_required (VERSION 2.8)
project (thttpd-cgic)

include_directories (include)
#link_directories(/opt/local/lib)

#FIND_PATH(PCRE_INCLUDE_DIR NAMES pcre.h)
#FIND_LIBRARY(PCRE_LIBRARY NAMES pcre)

#find_package (PCRE REQUIRED)
#if (PCRE_FOUND)
#else ()
#	message (STATUS "PCRE not found")
#endif ()

#
# How To Compile SQLite: https://sqlite.org/howtocompile.html
# -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSIONI
#
#add_library(sqlite3
#	deps/sqlite/sqlite3.c
#)
#target_link_libraries(sqlite3
#	pthread
#	dl
#)
#add_executable(sqlite3_cmd
#	deps/sqlite/shell.c
#)
#target_link_libraries(sqlite3_cmd
#	sqlite3
#)

find_package(CURL)
if(CURL_FOUND)
	include_directories(${CURL_INCLUDE_DIR})
else(CURL_FOUND)
	message(FATAL_ERROR "libcurl not found")
endif(CURL_FOUND)

include_directories(deps/cgic-1.05/include)
add_library(cgic
	deps/cgic-1.05/src/cgic.c
)

add_executable(study-curl.cgi
	src/study-curl.c
)
target_link_libraries(study-curl.cgi
	cgic
	curl
)

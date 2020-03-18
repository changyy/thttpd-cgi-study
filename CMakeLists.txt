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

# https://raw.githubusercontent.com/boutell/cgic/master/cgic.h
# https://raw.githubusercontent.com/boutell/cgic/master/cgic.c
#set(CGIC_VERSION "cgic-1.05")
set(CGIC_VERSION "cgic-2.07")

include_directories(deps/${CGIC_VERSION}/include)
add_library(cgic
	deps/${CGIC_VERSION}/src/cgic.c
)

# https://raw.githubusercontent.com/DaveGamble/cJSON/v1.7.12/cJSON.h
# https://raw.githubusercontent.com/DaveGamble/cJSON/v1.7.12/cJSON.c
set(CJSON_VERSION "cjson-1.7.12")
include_directories(deps/${CJSON_VERSION}/include)
add_library(cjson
	deps/${CJSON_VERSION}/src/cJSON.c
)

add_executable(study-curl.cgi
	src/study-curl.c
)
target_link_libraries(study-curl.cgi
	cgic
	curl
)

add_executable(study-cjson.cgi
	src/study-cjson.c
)
target_link_libraries(study-cjson.cgi
	cgic
	cjson
)

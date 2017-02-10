cmake_minimum_required (VERSION 3.0 FATAL_ERROR)

project (mlnetwork LANGUAGES CXX)
set (PROJECT_VENDOR 			"Matteo Magnani")
set (PROJECT_CONTACT 			"matteo.magnani@it.uu.se")
set (PROJECT_URL 				"https://github.com/magnanim/multinet")
set (PROJECT_DESCRIPTION 		"mlnetwork")
set (CMAKE_BUILD_TYPE 			"Release")

file (READ 						"${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PROJECT_VERSION_FULL)
string (REGEX REPLACE 			"[\n\r]" "" PROJECT_VERSION_FULL "${PROJECT_VERSION_FULL}")
string (REGEX REPLACE 			"^([0-9]+)\\.[0-9]+\\.[0-9]+$" "\\1" PROJECT_VERSION_MAJOR "${PROJECT_VERSION_FULL}")
string (REGEX REPLACE 			"^[0-9]+\\.([0-9]+)\\.[0-9]+$" "\\1" PROJECT_VERSION_MINOR "${PROJECT_VERSION_FULL}")
string (REGEX REPLACE 			"^[0-9]+\\.[0-9]+\\.([0-9]+)$" "\\1" PROJECT_VERSION_PATCH "${PROJECT_VERSION_FULL}")

set (PROJECT_VERSION 			"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
math (EXPR LIBRARY_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set (LIBRARY_VERSION_MINOR 		"${PROJECT_VERSION_MINOR}")
set (LIBRARY_VERSION_PATCH 		"${PROJECT_VERSION_PATCH}")
set (LIBRARY_VERSION 			"${LIBRARY_VERSION_MAJOR}.${LIBRARY_VERSION_MINOR}")
set (LIBRARY_VERSION_FULL 		"${LIBRARY_VERSION}.${LIBRARY_VERSION_PATCH}")

if (CMAKE_CXX_COMPILER MATCHES ".*clang.*" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set (CMAKE_COMPILER_IS_CLANGXX 1)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	set(CMAKE_COMPILER_IS_INTEL 1)
endif()

if ( (CMAKE_COMPILER_IS_GNUCXX AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.7) OR
	 (CMAKE_COMPILER_IS_CLANGXX AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.2) )
	 	message (FATAL_ERROR "Your C++ compiler does not support C++11. Please install g++ 4.7 (or greater) or clang 3.2 (or greater)")
else()
		message (STATUS "Compiler is recent enough to support C++11.")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -DNDEBUG")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O4 -fmessage-length=0 -funroll-loops -fsanitize=address -fno-omit-frame-pointer")

else()
	if (CMAKE_COMPILER_IS_CLANGXX)
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DNDEBUG")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O4 -fmessage-length=0 -funroll-loops -fsanitize=address -fno-omit-frame-pointer -D__extern_always_inline=\"extern __always_inline\"")

	else()
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O4 -fmessage-length=0 -funroll-loops -fsanitize=address -no-inline-min-size -no-inline-max-size")
	endif()
endif()

include_directories (include)
file (GLOB_RECURSE SOURCES src/*.cpp)

#find_package (Doxygen)
#if (DOXYGEN_FOUND)
#	set (DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/multiplenetwork.doxyfile)
#	set (DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
#	configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
#	add_custom_target(doc ALL
#		COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
#		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#		COMMENT "Generating API documentation with Doxygen"
#		VERBATIM)
#else()
#	message (FATAL_ERROR "Doxygen is needed to build the documentation.")
#endif()

add_library (mlnetwork SHARED ${SOURCES})

install (TARGETS mlnetwork
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)


add_subdirectory(test)

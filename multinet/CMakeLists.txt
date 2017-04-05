cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

project (multinet LANGUAGES CXX)
set (PROJECT_VENDOR 			"Matteo Magnani")
set (PROJECT_CONTACT 			"matteo.magnani@it.uu.se")
set (PROJECT_URL 				"https://github.com/magnanim/multinet")
set (PROJECT_DESCRIPTION 		"mlnetwork")
set (CMAKE_BUILD_TYPE 			"Debug")

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
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -lcblas -DNDEBUG")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O4 -fopenmp -fmessage-length=0 -funroll-loops -fsanitize=address -fno-omit-frame-pointer -march=native")
else()
	if (CMAKE_COMPILER_IS_CLANGXX)
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DNDEBUG -Wextra -Wall")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O4 -fmessage-length=0 -fsanitize=address -funroll-loops -fno-omit-frame-pointer -D__extern_always_inline=\"extern __always_inline\"")


	else()
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O4 -fmessage-length=0 -funroll-loops -fsanitize=address -no-inline-min-size -no-inline-max-size")
	endif()
endif()

include_directories (include)
include_directories (/usr/include/eigen3)

find_package(BLAS REQUIRED)
find_package(dlib REQUIRED)

file (GLOB_RECURSE SOURCES src/*.cpp)

option(DOXY "Generate DoxyGen documentation" OFF)
if(DOXY)
	find_package (Doxygen)
	if (DOXYGEN_FOUND)
		set (DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/multinet.doxyfile)
		set (DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
		configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
		add_custom_target(doc ALL
			COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Generating API documentation with Doxygen"
			VERBATIM)
	else()
		message (FATAL_ERROR "Doxygen is needed to build the documentation.")
	endif()
endif(DOXY)

set(PROBE_PATH "")
option(dtrace "Add dtrace support to target" OFF)
if(dtrace)
	if(NOT WIN32)
		execute_process(COMMAND which dtrace OUTPUT_VARIABLE out)
		if (NOT out)
			message(FATAL_ERROR "dtrace generator not found!")
		endif()

		file(GLOB providers "include/dtrace/providers/*.d")

		foreach(provider ${providers})
			get_filename_component(p ${provider} NAME_WE)
			execute_process(COMMAND dtrace -h -s ${provider} -o include/dtrace/headers/${p}.h)
		endforeach()

		if(UNIX AND NOT APPLE)
			file(MAKE_DIRECTORY include/dtrace/objects)
			foreach(provider ${providers})
				get_filename_component(p ${provider} NAME_WE)
				execute_process(COMMAND dtrace -G -s include/dtrace/providers/${p}.d -o include/dtrace/objects/${p}.o)
				set(PROBE_PATH include/dtrace/objects/${p}.o)
			endforeach()
		endif()

		add_definitions(-DUSE_DYNAMIC_TRACE)
	endif()
endif(dtrace)

add_library (multinet SHARED ${SOURCES} ${dlib_LIBS} ${PROBE_PATH})

install (TARGETS multinet
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

add_subdirectory(test)



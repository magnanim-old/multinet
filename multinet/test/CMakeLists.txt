#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../include
#	${CMAKE_CURRENT_SOURCE_DIR}/include)
#file (GLOB_RECURSE SOURCES_TEST ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp
#	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../include)
file (GLOB_RECURSE SOURCES_TEST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable (mlnetwork-tests ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SOURCES_TEST})
target_link_libraries (mlnetwork-tests mlnetwork)

add_custom_command(TARGET mlnetwork-tests
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:mlnetwork-tests>
	COMMENT "Copying testing datasets."
	VERBATIM)

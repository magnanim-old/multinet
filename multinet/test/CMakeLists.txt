#include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../include
#	${CMAKE_CURRENT_SOURCE_DIR}/include)
#file (GLOB_RECURSE SOURCES_TEST ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp
#	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../include)
file (GLOB_RECURSE SOURCES_TEST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable (multinet-tests ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SOURCES_TEST})
target_link_libraries (multinet-tests multinet -ldlib -lblas -lboost_serialization)

add_custom_command(TARGET multinet-tests
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:multinet-tests>
	COMMENT "Copying testing datasets."
	VERBATIM)

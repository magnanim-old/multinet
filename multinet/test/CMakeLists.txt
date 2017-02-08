include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../include
	${CMAKE_CURRENT_SOURCE_DIR}/include)
file (GLOB_RECURSE SOURCES_TEST ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(mlnetwork-tests ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${SOURCES_TEST})

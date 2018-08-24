cmake_minimum_required(VERSION 2.6)
project(netlib)

# Ensure all dependencies are existent.
execute_process(
	COMMAND git submodule init
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
execute_process(
	COMMAND git submodule update
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(depend/libcr)

# Set the C++ standard used by the netlib.
set(CMAKE_CXX_FLAGS "-std=c++17 -Wfatal-errors -DNETLIB_BUILD")

# Select all source files.
file(GLOB_RECURSE netlib_sources ./src/*.cpp)

# Add them to the project.
add_library(netlib ${netlib_sources})

target_link_libraries(netlib libcr)

# Add the dependency libraries to the include directories
include_directories(depend/libcr/include)

# Creates an include directory containing all header files used in the netlib.
# Add /netlib/include/ to your include directories and access the files via #include <netlib/*>
file(COPY "src/" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/netlib/ FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
file(COPY "depend/libcr/include/" DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/netlib/)
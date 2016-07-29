cmake_minimum_required(VERSION 2.6)
project(netlib)

# Set the C++ standard used by the netlib.
set(CMAKE_CXX_FLAGS "-std=c++11 -Wfatal-errors -DNETLIB_BUILD")

# Select all source files.
file(GLOB_RECURSE netlib_sources ./src/*.cpp)
# Add them to the RmbRT Engine library.
add_library(netlib ${netlib_sources})

# Add the Lock library to the include directories
include_directories(depend/Lock/include)

# Creates an include directory containing all header files used in the netlib.
# Add /netlib/include/ to your include directories and access the files via #include <netlib/*>
file(COPY "src/" DESTINATION "include/netlib/" FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
file(COPY "depend/Lock/include/" DESTINATION "include/" FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
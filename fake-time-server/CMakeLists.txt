cmake_minimum_required(VERSION 3.10)

# Project name
project(MetricsServer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/cpp-httplib)

# Add executable
add_executable(metrics_server server.cpp)

# Link libraries
target_link_libraries(metrics_server pthread)



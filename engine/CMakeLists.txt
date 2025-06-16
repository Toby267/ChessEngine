cmake_minimum_required(VERSION "3.25.1")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)

project(ChessEngine)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(ChessEngine "${MY_SOURCES}")
target_include_directories(ChessEngine PUBLIC "${CMAKE_SOURCE_DIR}/include")

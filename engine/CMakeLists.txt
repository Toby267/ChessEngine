cmake_minimum_required(VERSION "3.25.1")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 20)

project(ChessEngine)
add_executable(ChessEngine "${CMAKE_SOURCE_DIR}/src/Engine.cpp")
target_include_directories(ChessEngine PUBLIC "${CMAKE_SOURCE_DIR}/include")

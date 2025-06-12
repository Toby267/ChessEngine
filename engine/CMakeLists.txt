cmake_minimum_required(VERSION "3.25.1")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

project("ChessEngine")
add_executable(ChessEngine "src/Engine.cpp")
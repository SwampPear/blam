cmake_minimum_required(VERSION 3.20)
project(blamc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Header-only libs (tokens, lexer, utils) in include/
add_library(blamlib INTERFACE)
target_include_directories(blamlib INTERFACE ${CMAKE_SOURCE_DIR}/include)

add_executable(blamc src/main.cpp)
target_link_libraries(blamc PRIVATE blamlib)

if (MSVC)
  target_compile_options(blamc PRIVATE /W4 /permissive-)
else()
  target_compile_options(blamc PRIVATE -Wall -Wextra -Wpedantic)
endif()

message(STATUS "Headers: ${CMAKE_SOURCE_DIR}/include/{tokens.hpp,lexer.hpp,utils.hpp}")
message(STATUS "Example file: ${CMAKE_SOURCE_DIR}/examples/hello.blam")

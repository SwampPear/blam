cmake_minimum_required(VERSION 3.20)
project(blamc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Header-only includes in include/
add_library(blamlib INTERFACE)
target_include_directories(blamlib INTERFACE ${CMAKE_SOURCE_DIR}/include)

# Semantic analysis implementation
add_library(blam_sem STATIC src/sem.cpp)
# Reuse include path via blamlib
target_link_libraries(blam_sem PUBLIC blamlib)

# Executable
add_executable(blamc src/main.cpp)
target_link_libraries(blamc PRIVATE blamlib blam_sem)

# Warnings
if (MSVC)
  target_compile_options(blamc PRIVATE /W4 /permissive-)
  target_compile_options(blam_sem PRIVATE /W4 /permissive-)
else()
  target_compile_options(blamc PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(blam_sem PRIVATE -Wall -Wextra -Wpedantic)
endif()

message(STATUS "Headers: ${CMAKE_SOURCE_DIR}/include/{tokens.hpp,lexer.hpp,utils.hpp}")
message(STATUS "Example file: ${CMAKE_SOURCE_DIR}/examples/hello.blam")

cmake_minimum_required(VERSION 3.16)
project(Blam)

# Include directories
include_directories(include)

# Gather source files relative to root
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

# Static library
add_library(blam STATIC ${SRC_FILES})

# Catch2 (fetch and link)
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)

# tests
add_executable(test_tokenizer tests/test_tokenizer.cpp)
target_link_libraries(test_tokenizer PRIVATE blam Catch2::Catch2WithMain)

add_executable(test_parser tests/test_parser.cpp)
target_link_libraries(test_parser PRIVATE blam Catch2::Catch2WithMain)

# main
add_executable(blam_main main.cpp)
target_link_libraries(blam_main PRIVATE blam)

# enable testing
include(CTest)
include(Catch)
catch_discover_tests(test_tokenizer)
catch_discover_tests(test_parser)
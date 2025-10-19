cmake_minimum_required(VERSION 3.22)
project(blam LANGUAGES C CXX)

# Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# LLVM
#find_package(LLVM REQUIRED CONFIG)
#message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
#message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
#message(STATUS "LLVM definitions: ${LLVM_DEFINITIONS}")
#llvm_map_components_to_libnames(LLVM_LIBS core irreader support)

# Project library (shareable between app and tests)
add_library(blam STATIC
  src/lexer/lexer.cpp
)

target_include_directories(blam PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${LLVM_INCLUDE_DIRS}
)

target_compile_definitions(blam PUBLIC ${LLVM_DEFINITIONS}
  __STDC_CONSTANT_MACROS
  __STDC_FORMAT_MACROS
  __STDC_LIMIT_MACROS
)

target_link_libraries(blam PUBLIC ${LLVM_LIBS})

add_executable(blamc src/main.cpp)
target_link_libraries(blamc PRIVATE blam)

# Tests (Catch2 v3)
find_package(Catch2 3 REQUIRED)
add_executable(tests
  tests/test_lexer.cpp
)
include(CTest)                 # enables ctest
find_package(Catch2 3 REQUIRED)
add_executable(blamtests tests/test_lexer.cpp)   # etc.
target_link_libraries(blamtests PRIVATE blam Catch2::Catch2WithMain)
include(Catch)
catch_discover_tests(tests)    # registers tests with CTest

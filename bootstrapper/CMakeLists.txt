cmake_minimum_required(VERSION 3.22)
project(blam LANGUAGES C CXX)

# Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM definitions: ${LLVM_DEFINITIONS}")
llvm_map_components_to_libnames(LLVM_LIBS core irreader support)

# Project library (shareable between app and tests)
add_library(blam STATIC
  src/codegen_llvm.cpp
  src/sem.cpp
  # add other .cpp files here (e.g. parser, lexer, etc.)
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

# CLI executable
add_executable(blamc src/main.cpp)
target_link_libraries(blamc PRIVATE blam)

# --- Testing (Catch2 + CTest) ---
include(CTest)
if(BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.6.0
  )
  FetchContent_MakeAvailable(Catch2)
  add_executable(blam_tests
    tests/test_lexer.cpp
    # add more test files here
  )
  target_link_libraries(blam_tests PRIVATE blam Catch2::Catch2WithMain)
  target_include_directories(blam_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
  include(Catch)
  catch_discover_tests(blam_tests)
endif()

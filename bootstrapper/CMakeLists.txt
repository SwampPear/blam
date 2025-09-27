cmake_minimum_required(VERSION 3.22)
project(blam LANGUAGES CXX)

cmake_minimum_required(VERSION 3.22)
project(blam LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)  # keep the rest of your LLVM setup as-is

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM definitions: ${LLVM_DEFINITIONS}")

# Pick a minimal set of components you actually use
llvm_map_components_to_libnames(LLVM_LIBS
  core
  irreader
  support
)

add_executable(blamc
  src/main.cpp
  src/codegen_llvm.cpp
  src/sem.cpp
  # add your other .cpp files here
)

# LLVM include paths & defs
target_include_directories(blamc PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(blamc PRIVATE ${LLVM_DEFINITIONS})
# (often useful for LLVM headers)
target_compile_definitions(blamc PRIVATE
  __STDC_CONSTANT_MACROS
  __STDC_FORMAT_MACROS
  __STDC_LIMIT_MACROS
)

# Your own include dir(s)
target_include_directories(blamc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link LLVM
target_link_libraries(blamc PRIVATE ${LLVM_LIBS})

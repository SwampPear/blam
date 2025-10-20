cmake_minimum_required(VERSION 3.20)
project(YourProject LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Warns
if(MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Your library/app sources (optional). Comment out if you don't have src yet.
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp)
if(PROJECT_SOURCES)
  add_library(project_lib STATIC ${PROJECT_SOURCES})
  target_include_directories(project_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)
endif()

# ---- GoogleTest via FetchContent ----
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
)
# For MSVC: avoid overriding parent CRT flags
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ---- Tests ----
enable_testing()
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
  tests/*_test.cpp tests/*Test.cpp tests/*.cpp
)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests PRIVATE GTest::gtest GTest::gtest_main)
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(TARGET project_lib)
  target_link_libraries(tests PRIVATE project_lib)
endif()
add_test(NAME all COMMAND tests)

cmake_minimum_required(VERSION 3.14)
project(Blam LANGUAGES CXX)

include(FetchContent)  # <-- Add this line BEFORE calling FetchContent_Declare

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4  # Or latest tag
)

FetchContent_MakeAvailable(Catch2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source and header directories
include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)
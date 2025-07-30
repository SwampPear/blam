include(FetchContent)

# Fetch Catch2 (header-only)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4 # or latest stable version
)

FetchContent_MakeAvailable(Catch2)

# Add test executable
add_executable(test_tokenizer test_tokenizer.cpp)
target_link_libraries(test_tokenizer PRIVATE blam Catch2::Catch2WithMain)

# Optionally enable CTest
include(CTest)
include(Catch)
catch_discover_tests(test_tokenizer)

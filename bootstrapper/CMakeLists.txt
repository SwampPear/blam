cmake_minimum_required(VERSION 3.20)

# Enable both C and C++ (LLVM's CMake does some C-based try_compile checks)
project(blamc LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Toggle: try to build with real LLVM backend (OFF uses stub regardless of LLVM presence)
option(BLAM_ENABLE_LLVM "Build with real LLVM backend if found" ON)

# ---- Includes (header-only) ----
add_library(blamlib INTERFACE)
target_include_directories(blamlib INTERFACE ${CMAKE_SOURCE_DIR}/include)

# ---- Semantic analyzer ----
add_library(blam_sem STATIC src/sem.cpp)
target_link_libraries(blam_sem PUBLIC blamlib)

# ---- Executable ----
add_executable(blamc
  src/main.cpp
  src/codegen_llvm.cpp   # contains both real + stub implementations (guarded by BLAM_WITH_LLVM)
)
target_link_libraries(blamc PRIVATE blamlib blam_sem)

# ---- LLVM wiring (robust) ----
if (BLAM_ENABLE_LLVM)
  # Try config-mode first (recommended on Homebrew/macOS or packaged installs)
  find_package(LLVM QUIET CONFIG)

  if (LLVM_FOUND)
    message(STATUS "LLVM found: ${LLVM_PACKAGE_VERSION}")
    # Add LLVM's include dirs/definitions (for llvm-config.h, etc.)
    target_include_directories(blamc PRIVATE ${LLVM_INCLUDE_DIRS})
    target_compile_definitions(blamc PRIVATE ${LLVM_DEFINITIONS})

    # Prefer imported targets if available (modern LLVM)
    if (TARGET LLVM::Core)
      target_compile_definitions(blamc PRIVATE BLAM_WITH_LLVM)
      target_link_libraries(blamc PRIVATE LLVM::Core LLVM::IRReader LLVM::Support)
      message(STATUS "Using imported LLVM targets.")
    else()
      # Legacy fallback: map components to libs
      include(LLVMConfig)      # provides llvm_map_components_to_libnames
      llvm_map_components_to_libnames(LLVM_LIBS Core IRReader Support)
      if (LLVM_LIBS)
        target_compile_definitions(blamc PRIVATE BLAM_WITH_LLVM)
        target_link_libraries(blamc PRIVATE ${LLVM_LIBS})
        message(STATUS "Using legacy LLVM component libraries: ${LLVM_LIBS}")
      else()
        message(WARNING "LLVM found but could not resolve component libs; building stubbed codegen.")
      endif()
    endif()
  else()
    message(STATUS "LLVM not found; building stubbed codegen (no real IR).")
  endif()
else()
  message(STATUS "BLAM_ENABLE_LLVM=OFF — building stubbed codegen.")
endif()

# ---- Warnings ----
if (MSVC)
  target_compile_options(blamc PRIVATE /W4 /permissive-)
  target_compile_options(blam_sem PRIVATE /W4 /permissive-)
else()
  target_compile_options(blamc PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(blam_sem PRIVATE -Wall -Wextra -Wpedantic)
endif()

message(STATUS "Headers: ${CMAKE_SOURCE_DIR}/include/{tokens.hpp,lexer.hpp,utils.hpp}")
message(STATUS "Example file: ${CMAKE_SOURCE_DIR}/examples/hello.blam")

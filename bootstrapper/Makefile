# Makefile

.PHONY: all configure build clean basic_test advanced_test reconfigure

# --- Homebrew LLVM location (used by CMake's find_package(LLVM)) ---
LLVM_PREFIX := $(shell brew --prefix llvm 2>/dev/null)
LLVM_DIR    := $(LLVM_PREFIX)/lib/cmake/llvm

# --- Compiler selection ---
# Default: use Apple toolchain to avoid libc++ ABI mismatches.
USE_BREW_CLANG ?= 0

ifeq ($(USE_BREW_CLANG),1)
  CCOMP  := $(LLVM_PREFIX)/bin/clang
  CXXCOMP:= $(LLVM_PREFIX)/bin/clang++
  # Link against Homebrew's libc++ when using brew clang++
  EXTRA_LINK_OPTS := -nostdlib++ -L"$(LLVM_PREFIX)/lib" -Wl,-rpath,"$(LLVM_PREFIX)/lib" -lc++ -lc++abi
  EXTRA_INC_OPTS  := -nostdinc++ -I"$(LLVM_PREFIX)/include/c++/v1"
  CMAKE_COMPILERS := -DCMAKE_C_COMPILER="$(CCOMP)" -DCMAKE_CXX_COMPILER="$(CXXCOMP)"
  CMAKE_LINK_OPTS := -DCMAKE_EXE_LINKER_FLAGS_INIT='$(EXTRA_LINK_OPTS)'
  CMAKE_CXXFLAGS  := -DCMAKE_CXX_FLAGS_INIT='$(EXTRA_INC_OPTS)'
else
  # Use system /usr/bin/clang and /usr/bin/clang++
  CMAKE_COMPILERS :=
  CMAKE_LINK_OPTS :=
  CMAKE_CXXFLAGS  :=
endif

all: build

configure:
	cmake -S . -B build \
	  -DCMAKE_BUILD_TYPE=Debug \
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	  -DLLVM_DIR="$(LLVM_DIR)" \
	  $(CMAKE_COMPILERS) \
	  $(CMAKE_LINK_OPTS) \
	  $(CMAKE_CXXFLAGS)

build: configure
	cmake --build build -j

reconfigure:
	rm -rf build
	$(MAKE) build

clean:
	rm -rf build out.ll

basic_test: build
	./build/blamc examples/basic.blam --emit-ll out.ll
	sed -n '1,80p' out.ll

advanced_test: build
	./build/blamc examples/advanced.blam --emit-ll out.ll
	sed -n '1,80p' out.ll

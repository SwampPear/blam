# Makefile

.PHONY: all configure build clean distclean reconfigure \
        debug release run test basic_test advanced_test format print-%

# ---- Settings ----
BUILD_DIR ?= build
CONFIG    ?= Debug            # Debug | Release | RelWithDebInfo | MinSizeRel
GEN       ?=                  # e.g., -G "Unix Makefiles" (optional)
NPROC     ?= $(shell sysctl -n hw.ncpu 2>/dev/null || nproc)

# ---- Homebrew LLVM (used by CMake's find_package(LLVM)) ----
LLVM_PREFIX := $(shell brew --prefix llvm 2>/dev/null)
LLVM_DIR    := $(LLVM_PREFIX)/lib/cmake/llvm

# ---- Compiler selection ----
# Default: use Apple toolchain to avoid libc++ ABI mismatches.
USE_BREW_CLANG ?= 0

ifeq ($(USE_BREW_CLANG),1)
  CCOMP           := $(LLVM_PREFIX)/bin/clang
  CXXCOMP         := $(LLVM_PREFIX)/bin/clang++
  EXTRA_LINK_OPTS := -nostdlib++ -L"$(LLVM_PREFIX)/lib" -Wl,-rpath,"$(LLVM_PREFIX)/lib" -lc++ -lc++abi
  EXTRA_INC_OPTS  := -nostdinc++ -I"$(LLVM_PREFIX)/include/c++/v1"
  CMAKE_COMPILERS := -DCMAKE_C_COMPILER="$(CCOMP)" -DCMAKE_CXX_COMPILER="$(CXXCOMP)"
  CMAKE_LINK_OPTS := -DCMAKE_EXE_LINKER_FLAGS_INIT='$(EXTRA_LINK_OPTS)'
  CMAKE_CXXFLAGS  := -DCMAKE_CXX_FLAGS_INIT='$(EXTRA_INC_OPTS)'
else
  CMAKE_COMPILERS :=
  CMAKE_LINK_OPTS :=
  CMAKE_CXXFLAGS  :=
endif

# ---- Default target ----
all: build

# ---- Configure & Build ----
configure:
	cmake -S . -B $(BUILD_DIR) \
	  -DCMAKE_BUILD_TYPE=$(CONFIG) \
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	  -DLLVM_DIR="$(LLVM_DIR)" \
	  -DBUILD_TESTING=ON \
	  $(GEN) \
	  $(CMAKE_COMPILERS) \
	  $(CMAKE_LINK_OPTS) \
	  $(CMAKE_CXXFLAGS)

build: configure
	cmake --build $(BUILD_DIR) --parallel $(NPROC)

reconfigure:
	rm -rf $(BUILD_DIR)
	$(MAKE) build

# ---- Profiles ----
debug:
	$(MAKE) CONFIG=Debug all

release:
	$(MAKE) CONFIG=Release all

# ---- Run & Test ----
# Usage: make run ARG=examples/advanced.blam
run: build
	$(BUILD_DIR)/blamc $(ARG)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

# ---- One-off demo targets (preserved) ----
basic_test: build
	$(BUILD_DIR)/blamc examples/basic.blam --emit-ll out.ll
	sed -n '1,120p' out.ll

advanced_test: build
	$(BUILD_DIR)/blamc examples/advanced.blam --emit-ll out.ll
	sed -n '1,120p' out.ll

# ---- Hygiene ----
clean:
	- cmake --build $(BUILD_DIR) --target clean || true
	- rm -f out.ll

distclean:
	rm -rf $(BUILD_DIR) out.ll

format:
	clang-format -i $(shell git ls-files '*.[ch]pp' '*.hpp' 2>/dev/null)

print-%:
	@echo $*=$($*)

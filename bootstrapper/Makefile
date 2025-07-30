CC = g++

TARGET = build/main
INCLUDE = include
INCLUDE_PATH = src
SUB_DIR = src src/core src/tokenizer src/parser src/codegen

LLVM_CONFIG := $(shell command -v llvm-config 2>/dev/null)
LLVM_FLAGS := $(if $(LLVM_CONFIG),$(shell $(LLVM_CONFIG) --cxxflags --ldflags --system-libs --libs core),)

CFLAGS = -std=c++17 -Wall -I $(INCLUDE) -I $(INCLUDE_PATH) $(LLVM_FLAGS) -fexceptions

all: $(TARGET)

$(TARGET): main.cpp $(wildcard $(foreach fd, ${SUB_DIR}, $(fd)/*.cpp)) $(wildcard $(foreach fd, ${SUB_DIR}, $(fd)/*.hpp))
	$(CC) $(CFLAGS) -o $(TARGET) main.cpp $(wildcard $(foreach fd, ${SUB_DIR}, $(fd)/*.cpp))

clean:
	$(RM) $(TARGET)
	echo "$(wildcard $(foreach fd, ${SUB_DIR}, $(fd)/*.hpp))"

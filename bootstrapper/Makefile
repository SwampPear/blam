CC = g++

TARGET = build/main
INCLUDE = include
INCLUDE_PATH = src
SUB_DIR = src src/tokenizer src/parser src/codegen

CFLAGS = -std=c++17 -Wall -I $(INCLUDE) -I $(INCLUDE_PATH) `llvm-config --cxxflags --ldflags --system-libs --libs core`

all: $(TARGET)

$(TARGET): main.cpp $(wildcard $(foreach fd, ${SUB_DIR}, $(fd)/*.cpp)) $(wildcard $(foreach fd, ${SUB_DIR}, $(fd)/*.hpp))
	$(CC) $(CFLAGS) -o $(TARGET) main.cpp $(wildcard $(foreach fd, ${SUB_DIR}, $(fd)/*.cpp))

clean:
	$(RM) $(TARGET)
	echo "$(wildcard $(foreach fd, ${SUB_DIR}, $(fd)/*.hpp))"

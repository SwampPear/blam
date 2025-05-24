CC = g++

<<<<<<< HEAD
TARGET = build/main
INCLUDE = include
INCLUDE_PATH = src
SUB_DIR = src src/tokenizer src/parser src/codegen

CFLAGS = -std=c++17 -Wall  -I $(INCLUDE) -I $(INCLUDE_PATH) `llvm-config --cxxflags --ldflags --system-libs --libs core` -fexceptions

all: $(TARGET)

$(TARGET): main.cpp $(wildcard $(foreach fd, ${SUB_DIR}, $(fd)/*.cpp)) $(wildcard $(foreach fd, ${SUB_DIR}, $(fd)/*.hpp))
	$(CC) $(CFLAGS) -o $(TARGET) main.cpp $(wildcard $(foreach fd, ${SUB_DIR}, $(fd)/*.cpp))

clean:
	$(RM) $(TARGET)
	echo "$(wildcard $(foreach fd, ${SUB_DIR}, $(fd)/*.hpp))"
=======
TARGET = bootstrapper
SRC_PATH = src
SUB_DIR = src

CFLAGS = -std=c++17 -Wall -I $(SRC_PATH)

all: $(TARGET)

$(TARGET): $(wildcard *.cpp $(foreach fd, ${SUB_DIR}, $(fd)/*.cpp)) $(wildcard *.hpp $(foreach fd, ${SUB_DIR}, $(fd)/*.hpp))
	$(CC) $(CFLAGS) -o $(TARGET) $(wildcard *.cpp $(foreach fd, ${SUB_DIR}, $(fd)/*.cpp))

clean:
	$(RM) $(TARGET)
>>>>>>> b04542e597f676020691a300e91cef4083ba7a18

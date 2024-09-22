CC = g++

TARGET = bootstrapper
INCLUDE_PATH = src
SUB_DIR = src

CFLAGS = -std=c++17 -Wall -I $(SRC_PATH)

all: $(TARGET)

$(TARGET): $(wildcard *.cpp $(foreach fd, ${SUB_DIR}, $(fd)/*.cpp)) $(wildcard *.hpp $(foreach fd, ${SUB_DIR}, $(fd)/*.hpp))
	$(CC) $(CFLAGS) -o $(TARGET) $(wildcard *.cpp $(foreach fd, ${SUB_DIR}, $(fd)/*.cpp))

clean:
	$(RM) $(TARGET)
	echo "$(wildcard *.hpp $(foreach fd, ${SUB_DIR}, $(fd)/*.hpp))"

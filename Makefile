.PHONY: all clean build run test

all: build

clean:
	rm -rf build/src build/tests

build:
	cmake -B build
	cmake --build build -- -j$(nproc)

run:
	./build/promptengine_main

test:
	./build/test_tokenizer
	./build/test_parser

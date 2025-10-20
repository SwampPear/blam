.phony:
	make clean
	make build

build:
	make clean
	cmake -S . -B build
	cmake --build build -j

test:
	ctest --test-dir build --output-on-failure

clean:
	rm -rf build/
compile:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DLLVM_DIR="$(brew --prefix llvm)/lib/cmake/llvm"
	cmake --build build -j

basic_test:
	./build/blamc examples/basic.blam --emit-ll out.ll
	sed -n '1,80p' out.ll

advanced_test:
	./build/blamc examples/advanced.blam --emit-ll out.ll
	sed -n '1,80p' out.ll
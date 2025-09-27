cmake --build build -j
./build/blamc examples/hello.blam --emit-ll out.ll
sed -n '1,80p' out.ll
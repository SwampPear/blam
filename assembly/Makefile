main: main.o
	ld -o main main.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64 

main.o: main.s
	as -arch arm64 -o main.o main.s

machine:
	nasm -O0 -f bin -o machine main.s
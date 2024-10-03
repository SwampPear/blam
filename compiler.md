# Compiler

1. locate from src file from parent dir and file name (should begin with cwd and main.blam)
_dir_path = cwd
_src_path = main.blam

2. read src file and tokenize contents into token tree
_src_contents = read(_src_path)
_token_tree = tokenize(_src_path)

3. go function by function and parse dependencies from each function
> main.blam
import add, sub from math

add_two(i8 a) {
    return add(2, a)
}

add_three(i8 a) {
    sub(4, 3)       // does nothing
    return add(3, a)
}

main() {
    add_two()
    add()
}

> math.blam
add(i8 a, i8 b) -> i8 {
    return a + b
}

sub(i8 a, i8 b) -> i8 {
    return a - b
}

// a linker registry is a mapping of functions to dependencies within a single source file
// a + indicated the root function
linker registry:
- main.add_two [math.add]
- main.add_three [math.add, math.sub]
+ main.main [math.add, main.add_two]

4. shake duplicate dependencies from linker registry into linker tree node
linker registry:
- main.add_two [math.add]
- main.add_three [math.add, math.sub]
+ main.main [math.add, main.add_two]

linker registry:
- main.add_two [math.add]
+ main.main [math.add, main.add_two]

linker registry:
- main.add_two [math.add]
+ main.main [math.add, main.add_two, math.add]

linker registry:
+ main.main [math.add, main.add_two]

// the final shaked linker registry represents a linker node

5. 

4. repeat 1-3 recursively until no imports are found (scrub any duplicate imports)
5. tokenize down

compilation should be done by llvm and linking should be with lld
but should be intermediately be executed with as and lld to get bootstrapper working
IR should be general assembly

1. tokenization
2. semantic analysis (asts)
3. optimization
4. code generation
5. executable generation (linking, etc)
#include "driver.hpp"

int main() {
    BlamDriver::compile();

    return EXIT_SUCCESS;
}

/*

"""
1. locate from entry point (should start with main.blam in cwd)
2. tokenize
3. parse imports and module imports from rest of code
3.1. parse modules used in non-module
4. build linker tree
    recurse until no imports found
    1. locate from entry point (should be linked file)
    2. tokenize
    3. parse imports (should adhere to parsing rules)
"""
*/
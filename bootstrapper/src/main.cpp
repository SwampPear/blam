#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>
#include "utils.hpp"
#include "tokenizer.hpp"

int main() {
    // 1. locate from src file from parent dir and file name (should begin with cwd and main.blam)
    std::filesystem::path cwd = getCWD();
    std::string src = "main.blam";

    // 2. read src file and tokenize contents into token tree
    std::string srcContents = readFile(cwd/src);

    std::cout << srcContents << std::endl;

    return srcContents.foo()

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
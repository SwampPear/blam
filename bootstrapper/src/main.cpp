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

    return EXIT_SUCCESS;
}

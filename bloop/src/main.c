#include "stdio.h"

int main() {
    printf("Hello, World!\n");
    return 0;
}

/*
#include "analyzer.hpp"
#include "ir_compiler.hpp"
#include "utils.hpp"

namespace BlamDriver {

void compile() {
    // start from main
    std::string source_path = "src/main.blam";
    std::string source_contents = BlamUtils::read_file(source_path);
    std::string *ptr = &source_contents;

    // parse syntax
    BlamAnalyzer::Token *root = BlamAnalyzer::tokenize(ptr);
    std::cout << BlamAnalyzer::tokenToString(root, ptr, true);

    // generate intermediate representation from syntax trees
    // generate machine code and executable from IR
    //BlamIRCompiler::generateExecutable();
}

}  // BlamDriver
*/
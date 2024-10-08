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

#include <filesystem>
#include "lexi.hpp"

int main() {
    // read file to string
    std::string path = std::filesystem::current_path()/"example_project/main.blam";
    std::string src = Lexi::readFile(path);

    // build tokenizer
    Lexi::Tokenizer tokenizer = Lexi::Tokenizer();
    tokenizer.addRule("STRING", "\"[a-zA-Z0-9\\s\\}]*\"");
    tokenizer.addRule("SINGLE_LINE_COMMENT", "\\/\\/[\\sa-zA-Z0-9]*\n*");
    tokenizer.addRule("MULTI_LINE_COMMENT", "\\/\\*[\\sa-zA-Z0-9]*\\*\\/");
    tokenizer.addRule("L_DELIMETER", "\\(");
    tokenizer.addRule("R_DELIMETER", "\\)");
    tokenizer.addRule("L_CURLY_DELIMETER", "\\{");
    tokenizer.addRule("R_CURLY_DELIMETER", "\\}");
    tokenizer.addRule("L_SQUARE_DELIMETER", "\\[");
    tokenizer.addRule("R_SQUARE_DELIMETER", "\\]");
    tokenizer.addRule("OP_DOT", "\\.");
    tokenizer.addRule("OP_PLUS", "\\+");
    tokenizer.addRule("OP_MINUS", "\\-");
    tokenizer.addRule("OP_EQUALS", "=");
    tokenizer.addRule("OP_SLASH", "/");
    tokenizer.addRule("KEYWORD", "return");
    tokenizer.addRule("SPACE", "\\s+");
    
    // tokenize
    Lexi::TokenNode root = tokenizer.tokenize(&src);

    std::cout << tokenizer.tokenToString(root, true) << std::endl;

    return EXIT_SUCCESS;
}
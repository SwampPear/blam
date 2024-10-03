#pragma once

#include "lexi.hpp"
#include "ir_compiler.hpp"

namespace BlamDriver {

void compile() {
    // read file to string
    std::string path = "src/main.blam";     // begin from main file, later should search for this
    std::string src = Blam::readFile(path);

    // build tokenizer
    Lexi::Tokenizer tokenizer = Lexi::Tokenizer();
    tokenizer.addRule("STRING", "\"[a-zA-Z0-9\\s\\}]*\"");
    tokenizer.addRule("SINGLE_LINE_COMMENT", "\\/\\/[\\sa-zA-Z0-9]*\n");
    tokenizer.addRule("MULTI_LINE_COMMENT", "\\/\\*[\\sa-zA-Z0-9]*\\*\\/");
    tokenizer.addRule("L_DELIMETER", "\\(");
    tokenizer.addRule("R_DELIMETER", "\\)");
    tokenizer.addRule("L_CURLY_DELIMETER", "\\{");
    tokenizer.addRule("R_CURLY_DELIMETER", "\\}");
    tokenizer.addRule("L_SQUARE_DELIMETER", "\\[");
    tokenizer.addRule("R_SQUARE_DELIMETER", "\\]");
    tokenizer.addRule("OP_PLUS_EQUALS", "\\+=");
    tokenizer.addRule("OP_MINUS_EQUALS", "\\-=");
    tokenizer.addRule("OP_MULTIPLY_EQUALS", "\\*=");
    tokenizer.addRule("OP_DIVIDE_EQUALS", "/=");
    tokenizer.addRule("OP_ARROW", "->");
    tokenizer.addRule("OP_LEQ", "<=");
    tokenizer.addRule("OP_GEQ", ">=");
    tokenizer.addRule("OP_LT", "<");
    tokenizer.addRule("OP_GT", ">");
    tokenizer.addRule("OP_EQUALS", "=");
    tokenizer.addRule("OP_MINUS", "\\-");
    tokenizer.addRule("OP_DOT", "\\.");
    tokenizer.addRule("OP_PLUS", "\\+");
    tokenizer.addRule("OP_MINUS", "\\-");
    tokenizer.addRule("OP_STAR", "\\*");
    tokenizer.addRule("OP_SLASH", "/");
    tokenizer.addRule("OP_COMMA", ",");
    tokenizer.addRule("OP_SCOPE", "::");
    tokenizer.addRule("OP_COLON", ":");
    tokenizer.addRule("KEYWORD_RETURN", "return");
    tokenizer.addRule("KEYWORD_IMPORT", "import");
    tokenizer.addRule("KEYWORD_PUBLIC", "pub");
    tokenizer.addRule("KEYWORD_STRUCT", "struct");
    tokenizer.addRule("KEYWORD_FROM", "from");
    tokenizer.addRule("KEYWORD_CONST", "const");
    tokenizer.addRule("KEYWORD_CONST", "let");
    tokenizer.addRule("SPACE", "\\s+");
    tokenizer.addRule("ALPHANUM", "^[a-zA-Z][a-zA-Z0-9]+");
    tokenizer.addRule("NUM", "[0-9]+");
    
    // tokenize
    Lexi::TokenNode root = tokenizer.tokenize(&src);

    //std::cout << tokenizer.tokenToString(root, true) << std::endl;
    BlamIRCompiler::executable();
}

}  // Blam
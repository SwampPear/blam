#include "tokenizer.hpp"
#include "ir_compiler.hpp"
#include "utils.hpp"

namespace BlamDriver {

BlamTokenizer::TokenNode tokenize(std::string &path) {
    // read file to string

    std::string src = BlamUtils::readFile(path);

    // build tokenizer
    BlamTokenizer::Tokenizer tokenizer = BlamTokenizer::Tokenizer();
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
    tokenizer.addRule("ALPHANUM", "^[a-zA-Z][a-zA-Z0-9]*");
    tokenizer.addRule("NUM", "[0-9]+");
    
    // tokenize
    BlamTokenizer::TokenNode root = tokenizer.tokenize(&src);

    std::cout << tokenizer.tokenToString(root, true) << std::endl;

    return root;
}

void compile() {
    // start from main TODO: incorporate more robust file selection
    std::string path = "src/main.blam";

    // tokenize main file
    BlamTokenizer::TokenNode root = tokenize(path);

    // convert tokens into syntax tree
    // generate intermediate representation from syntax trees
    // generate machine code and executable from IR
    BlamIRCompiler::generateExecutable();
}

}  // BlamDriver
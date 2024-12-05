#include <stdio.h>
#include "tokenizer.h"

int main() {
    // test replace range
    token_t* token1 = create_token(NONE, 0, 1);
    token_t* token2 = create_token(STRING, 2, 3);
    token_t* token3 = create_token(SINGLE_LINE_COMMENT, 4, 5);
    token_t* token4 = create_token(MULTI_LINE_COMMENT, 6, 7);

    token1->next = token2;
    token2->prev = token1;
    token2->next = token3;
    token3->prev = token2;
    token3->next = token4;
    token4->prev = token3;

    token_t* token5 = create_token(L_DELIMETER, 8, 9);
    token_t* token6 = create_token(R_DELIMETER, 8, 9);

    token5->next = token6;
    token6->prev = token5;

    token_t* temp = replace_range(token2, token3, token5);

    // print tokens
    token_t* curr = token1;
    while (curr) {
        char* str = token_to_string(curr);
        printf("%s\n", str);
        curr = curr->next;
    }

    // print tokens
    curr = temp;
    while (curr) {
        char* str = token_to_string(curr);
        printf("%s\n", str);
        curr = curr->next;
    }

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
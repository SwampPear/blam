/*#include "src/tokenizer.hpp"

int main() {
    std::string fp = "example_project/src/main.blam";
    Tokenizer::tokenizeFile(fp);
    
    return 0;
}*/
/*
#include "parser/parser.hpp"
#include "codegen/codegen.hpp"
#include "tokenizer/tokenizer.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

int main() {
    std::string input = "3 + 4 * 2";

    auto tokens = Tokenizer::tokenize(input);
    printTokens(tokens, input);

    std::vector<Tokenizer::Token> tokenVec;
    for (auto curr = tokens; curr; curr = curr->next)
        tokenVec.push_back(*curr);

    size_t index = 0;
    auto ast = Parser::parseExpression(tokenVec, index);

    llvm::LLVMContext ctx;
    llvm::Module module("my_module", ctx);
    generateIR(ast, ctx, module);

    module.print(llvm::outs(), nullptr);
    return 0;
}
*/

#include <cstdlib>
#include <vector>
#include "utils.hpp"
#include "tokenizer/tokenizer.hpp"
#include "parser/parser.hpp"


int main() {
    std::string fp = "example_project/src/main.blam";
    std::string contents = Utils::readFile(fp);
    std::vector<Tokenizer::Token> tokens = Tokenizer::tokenizeFile(fp);
    Parser::parseProgram(tokens);

    Tokenizer::printTokens(tokens, contents);
}

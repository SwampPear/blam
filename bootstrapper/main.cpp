#include <cstdlib>
#include <vector>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include "utils.hpp"
#include "tokenizer/tokenizer.hpp"
#include "parser/parser.hpp"
#include "codegen/codegen.hpp"

using namespace BlamBootstrapper;

int main() {
    std::string fp = "example_project/src/main.blam";
    std::string contents = readFile(fp);
    std::vector<Token> tokens = tokenizeFile(fp);

    //printTokens(tokens, contents);
    std::unique_ptr<Stmt> program = parseProgram(tokens, contents);

    printAST(program, 0);

    generateIR(program);

    return EXIT_SUCCESS;
}

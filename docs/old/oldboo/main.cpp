#include <cstdlib>
#include <vector>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>

#include "utils.hpp"
#include "tokenizer/tokenizer.hpp"
#include "parser/parser.hpp"
#include "codegen/codegen.hpp"

using namespace BlamBootstrapper;

int main() {
    /*
    std::string fp = "example_project/src/main.blam";
    std::string contents = readFile(fp);
    std::vector<Token> tokens = tokenizeFile(fp);

    //printTokens(tokens, contents);
    std::unique_ptr<Stmt> program = parseProgram(tokens, contents);

    printAST(program, 0);

    generateIR(program);
    */

    llvm::LLVMContext ctx;
    llvm::Module module("SyscallModule", ctx);

    generateSyscallPrint(module, ctx);

    std::error_code ec;
    llvm::raw_fd_ostream outFile("output.ll", ec, llvm::sys::fs::OF_None);
    module.print(outFile, nullptr);  // writes human-readable LLVM IR
    outFile.close();

    //generateSyscallPrint(module, ctx);
    //module.print(llvm::outs(), nullptr); // emit IR

    return EXIT_SUCCESS;
}

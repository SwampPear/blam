#include <cstdlib>

#include "core/llist.hpp"
#include "core/utils.hpp"
#include "tokenizer/token.hpp"
#include "tokenizer/tokenizer.hpp"

using namespace Blam;

int main() {
    std::string fp = "compiler/src/main.blam";
    std::string src = readFile(fp);

    std::cout << "a" << std::endl;

    std::shared_ptr<LList<Token>> tokens = tokenize(src);
    /*
    std::vector<Token> tokens = tokenizeFile(fp);

    //printTokens(tokens, contents);
    std::unique_ptr<Stmt> program = parseProgram(tokens, contents);

    printAST(program, 0);

    generateIR(program);
    */

    /*
    llvm::LLVMContext ctx;
    llvm::Module module("SyscallModule", ctx);

    generateSyscallPrint(module, ctx);

    std::error_code ec;
    llvm::raw_fd_ostream outFile("output.ll", ec, llvm::sys::fs::OF_None);
    module.print(outFile, nullptr);  // writes human-readable LLVM IR
    outFile.close();
    */

    //generateSyscallPrint(module, ctx);
    //module.print(llvm::outs(), nullptr); // emit IR

    return EXIT_SUCCESS;
}

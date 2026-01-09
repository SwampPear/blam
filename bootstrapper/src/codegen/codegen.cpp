#include "codegen/codegen.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <system_error>

namespace Blam {

void generateIR(std::unique_ptr<Stmt> program, const std::string& outputPath) {
    auto scopedProgram = static_cast<ScopedStmt*>(program.get());

    llvm::LLVMContext context;
    llvm::Module module("blam", context);
    llvm::IRBuilder<> builder(context);

    for (const auto& stmt : scopedProgram->body) {
        std::cout << "Generating code for statement..." << std::endl;
        stmt->codegen(context, builder, module);
    }

    std::error_code ec;
    llvm::raw_fd_ostream outFile(outputPath, ec, llvm::sys::fs::OF_None);
    if (ec) {
        std::cerr << "Failed to write IR to " << outputPath << ": " << ec.message() << std::endl;
        return;
    }

    module.print(outFile, nullptr);
}

bool compileIRToExecutable(const std::string& irPath, const std::string& exePath, const std::string& clangPath) {
    std::ifstream probe(irPath);
    if (!probe.good()) {
        std::cerr << "IR file not found: " << irPath << std::endl;
        return false;
    }

    std::string compiler = clangPath.empty() ? "clang" : clangPath;
    std::string command = compiler + " " + irPath + " -o " + exePath;
    int rc = std::system(command.c_str());
    if (rc != 0) {
        std::cerr << "Failed to compile executable (rc=" << rc << "). Command: " << command << std::endl;
        return false;
    }

    return true;
}

}  // namespace Blam

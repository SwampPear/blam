#include "codegen/codegen.hpp"

namespace BlamBootstrapper {

void generateIR(std::unique_ptr<Stmt> program) {
    auto scopedProgram = static_cast<ScopedStmt*>(program.get());

    llvm::LLVMContext context;
    llvm::Module module(fp, context);
    llvm::IRBuilder<> builder(context);

    for (const auto& stmt : scopedProgram->body) {
        std::cout << "Generating code for statement..." << std::endl;
        stmt->codegen(context, builder, module);
    }

    module.print(llvm::outs(), nullptr);
}

}  // namespace BlamBootstrapper
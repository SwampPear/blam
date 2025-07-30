/*
#include "codegen/codegen.hpp"

namespace Blam {

void generateIR(std::unique_ptr<Stmt> program, std::string fp) {
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
    */

/*
void generateSyscallPrint(llvm::Module& module, llvm::LLVMContext& ctx) {
    llvm::IRBuilder<> builder(ctx);

    // define: int main()
    llvm::FunctionType* mainType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(ctx), false
    );
    llvm::Function* mainFunc = llvm::Function::Create(
        mainType, llvm::Function::ExternalLinkage, "main", module
    );

    // entry block
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(ctx, "entry", mainFunc);
    builder.SetInsertPoint(entry);

    // declare syscall: long syscall(long number, ...);
    llvm::FunctionType* syscallType = llvm::FunctionType::get(
        llvm::Type::getInt64Ty(ctx), { llvm::Type::getInt64Ty(ctx) }, true
    );
    llvm::FunctionCallee syscallFunc = module.getOrInsertFunction("syscall", syscallType);

    // message to print
    const char* msg = "Information provided is for educational purposes and not financial advice.\n";
    llvm::Value* msgPtr = builder.CreateGlobalStringPtr(msg);

    // syscall(SYS_write, 1, msg, len)
    llvm::Value* syscallNum = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 1); // SYS_write
    llvm::Value* fd = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), 1);         // stdout
    llvm::Value* len = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), strlen(msg));

    builder.CreateCall(syscallFunc, {syscallNum, fd, msgPtr, len});

    // return 0
    builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0));
}

}  // namespace Blam
  */
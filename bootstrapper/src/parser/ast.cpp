#include "parser/ast.hpp"
#include <map>

namespace BlamBootstrapper {

llvm::Value* NumberExpr::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) {
    return llvm::ConstantFP::get(ctx, llvm::APFloat(value));
}

llvm::Value* VarDeclStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) {
    llvm::Value* val = value->codegen(ctx, builder);
    if (!val) return nullptr;

    namedValues[name] = val;
    return val; // optional: could return void or store ptr
}

llvm::Value* FuncDeclStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) {
    std::cout << "Generating function: " << name << std::endl;
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        llvm::Type::getDoubleTy(ctx), // return type
        false // no args yet
    );

    std::cout << "Generating statement..." << std::endl;

    llvm::Function* func = llvm::Function::Create(
        funcType,       
        llvm::Function::ExternalLinkage,
        name,
        builder.GetInsertBlock()->getModule()
    );

    std::cout << "Generating statement..." << std::endl;

    llvm::BasicBlock* block = llvm::BasicBlock::Create(ctx, "entry", func);
    builder.SetInsertPoint(block);

    for (auto& stmt : body->body) {
        std::cout << "Generating statement..." << std::endl;
        stmt->codegen(ctx, builder);
    }

    builder.CreateRet(llvm::ConstantFP::get(ctx, llvm::APFloat(0.0)));

    return func;
}

llvm::Value* PubStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) {
    return body->codegen(ctx, builder);
}

void printAST(const std::unique_ptr<Stmt>& stmt, int indent = 0) {
    if (!stmt) return;

    std::string indentation(indent, ' ');

    switch (stmt->stmtType) {
        case StmtType::SCOPED: {
            auto pStmt = static_cast<ScopedStmt*>(stmt.get());
            std::cout << indentation << "ScopedStmt: " << std::endl;

            for (const auto& stmt : pStmt->body) {
                printAST(stmt, indent + 2);
            }

            break;
        }
        case StmtType::PUB: {
            auto pStmt = static_cast<PubStmt*>(stmt.get());
            std::cout << indentation << "PubStmt: " << std::endl;
            printAST(pStmt->body, indent + 2);

            break;
        }
        case StmtType::FUNC_DECL: {
            auto pStmt = static_cast<FuncDeclStmt*>(stmt.get());
            std::cout << indentation << "FuncDecl(" << pStmt->name << "): " << std::endl;

            for (const auto& stmt : pStmt->body->body) {
                printAST(stmt, indent + 2);
            }
            
            break;
        }
        case StmtType::VAR_DECL: {
            auto pStmt = static_cast<VarDeclStmt*>(stmt.get());
            std::cout << indentation << "VarDecl(" << pStmt->name << ")" << std::endl;
            break;
        }

        case StmtType::RETURN: {
            auto pStmt = static_cast<ReturnStmt*>(stmt.get());
            std::cout << indentation << "Return" << std::endl;
            break;
        }

        default:
            std::cout << indentation << "Unknown StmtType" << std::endl;
            break;
    }
}

llvm::Value* ScopedStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) {
    llvm::Value* val;
    return val;
}

llvm::Value* ExprStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) {
    llvm::Value* val;
    return val;
}

llvm::Value* ConstDeclStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) {
    llvm::Value* val;
    return val;
}

llvm::Value* ReturnStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) {
    llvm::Value* val;
    return val;
}

}  // namespace BlamBoostrapper
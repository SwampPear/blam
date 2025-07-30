/* #include "parser/ast.hpp"
#include <map>

namespace Blam {

llvm::Value* NumberExpr::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    return llvm::ConstantFP::get(ctx, llvm::APFloat(value));
}

llvm::Value* VarExpr::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    llvm::Value* val = namedValues[name];
    if (!val) {
        std::cerr << "Unknown variable name: " << name << std::endl;
        return nullptr;
    }
    return val;
}

llvm::Value* VarDeclStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    std::cout << "Generating code for variable declaration: " << name << std::endl;
    auto exprStmt = static_cast<ExprStmt*>(value.get());
    llvm::Value* val = exprStmt->expr->codegen(ctx, builder, module);
    if (!val) return nullptr;

    namedValues[name] = val;
    return val; // optional: could return void or store ptr
}

llvm::Value* FuncDeclStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    std::cout << "Generating function: " << name << std::endl;
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        llvm::Type::getDoubleTy(ctx), // return type
        false // no args yet
    );

    llvm::Function* func = llvm::Function::Create(
        funcType,       
        llvm::Function::ExternalLinkage,
        name,
        module
    );

    llvm::BasicBlock* block = llvm::BasicBlock::Create(ctx, "entry", func);
    builder.SetInsertPoint(block);

    for (auto& stmt : body->body) {
        std::cout << "Generating statement..." << std::endl;
        stmt->codegen(ctx, builder, module);
    }

    builder.CreateRet(llvm::ConstantFP::get(ctx, llvm::APFloat(0.0)));

    return func;
}

llvm::Value* PubStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    return body->codegen(ctx, builder, module);
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

llvm::Value* ScopedStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    return nullptr;
}

llvm::Value* ExprStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    return nullptr;
}

llvm::Value* ConstDeclStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    return nullptr;
}

llvm::Value* ReturnStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    std::cout << "Generating return" << std::endl;
    auto exprStmt = static_cast<ExprStmt*>(value.get());
    llvm::Value* val = exprStmt->expr->codegen(ctx, builder, module);
    if (!val) return nullptr;

    return builder.CreateRet(val);
}

}  // namespace Blam*/
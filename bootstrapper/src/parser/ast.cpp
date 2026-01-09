#include "parser/ast.hpp"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>

namespace {

llvm::Function* getOrCreatePuts(llvm::LLVMContext& ctx, llvm::Module& module) {
    if (auto* func = module.getFunction("puts")) {
        return func;
    }
    llvm::Type* i8Ptr = llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(ctx));
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(ctx),
        {i8Ptr},
        false
    );
    return llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        "puts",
        module
    );
}

llvm::Function* getOrCreatePrintf(llvm::LLVMContext& ctx, llvm::Module& module) {
    if (auto* func = module.getFunction("printf")) {
        return func;
    }
    llvm::Type* i8Ptr = llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(ctx));
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(ctx),
        {i8Ptr},
        true
    );
    return llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        "printf",
        module
    );
}

llvm::Value* createGlobalStringPtr(
    llvm::LLVMContext& ctx,
    llvm::IRBuilder<>& builder,
    llvm::Module& module,
    const std::string& value,
    const char* name
) {
    llvm::Value* zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 0);
    llvm::GlobalVariable* global = builder.CreateGlobalString(value, name, 0, &module);
    return builder.CreateInBoundsGEP(global->getValueType(), global, {zero, zero});
}

}  // namespace

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
    return val;
}

llvm::Value* FuncDeclStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    std::cout << "Generating function: " << name << std::endl;
    namedValues.clear();
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        llvm::Type::getDoubleTy(ctx),
        false
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

    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateRet(llvm::ConstantFP::get(ctx, llvm::APFloat(0.0)));
    }

    return func;
}

llvm::Value* PubStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    return body->codegen(ctx, builder, module);
}

void printAST(const std::unique_ptr<Stmt>& stmt, int indent) {
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
            std::cout << indentation << "Return" << std::endl;
            break;
        }
        case StmtType::PRINT: {
            std::cout << indentation << "Print" << std::endl;
            break;
        }
        default:
            std::cout << indentation << "Unknown StmtType" << std::endl;
            break;
    }
}

llvm::Value* ScopedStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    llvm::Value* last = nullptr;
    for (auto& stmt : body) {
        last = stmt->codegen(ctx, builder, module);
    }
    return last;
}

llvm::Value* ExprStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    if (!expr) {
        return nullptr;
    }
    return expr->codegen(ctx, builder, module);
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

llvm::Value* PrintStmt::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) {
    if (isString) {
        llvm::Function* putsFn = getOrCreatePuts(ctx, module);
        llvm::Value* strPtr = createGlobalStringPtr(ctx, builder, module, text, ".str");
        return builder.CreateCall(putsFn, {strPtr});
    }

    llvm::Function* printfFn = getOrCreatePrintf(ctx, module);
    llvm::Value* format = createGlobalStringPtr(ctx, builder, module, "%d\n", ".fmt");
    llvm::Value* value = llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), number, true);
    return builder.CreateCall(printfFn, {format, value});
}

}  // namespace Blam

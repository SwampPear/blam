#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <cstdint>

#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

namespace Blam {

inline std::map<std::string, llvm::Value*> namedValues;

struct Expr {
    virtual ~Expr() = default;
    virtual llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) = 0;
};

struct NumberExpr : Expr {
    double value;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) override;
};

struct VarExpr : Expr {
    std::string name;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) override;
};

enum class StmtType {
    PUB = 0,
    SCOPED,
    EXPR,
    FUNC_DECL,
    CONST_DECL,
    VAR_DECL,
    RETURN,
    PRINT
};

struct Stmt {
    StmtType stmtType;
    virtual ~Stmt() = default;
    virtual llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) = 0;
};

struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) override;
};

struct ScopedStmt : Stmt {
    std::string scope;
    std::vector<std::unique_ptr<Stmt>> body;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) override;
};

struct PubStmt : Stmt {
    std::unique_ptr<Stmt> body;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) override;
};

struct FuncDeclStmt : Stmt {
    std::string type;
    std::string name;
    std::unique_ptr<ScopedStmt> body;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) override;
};

struct ConstDeclStmt : Stmt {
    std::string type;
    std::string name;
    std::unique_ptr<Stmt> value;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) override;
};

struct VarDeclStmt : Stmt {
    std::string type;
    std::string name;
    std::unique_ptr<Stmt> value;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) override;
};

struct ReturnStmt : Stmt {
    std::unique_ptr<Stmt> value;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) override;
};

struct PrintStmt : Stmt {
    bool isString;
    std::string text;
    int64_t number;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder, llvm::Module& module) override;
};

void printAST(const std::unique_ptr<Stmt>& stmt, int indent);

}  // namespace Blam

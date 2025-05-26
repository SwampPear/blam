#pragma once

#include <memory>
#include <string>
#include <iostream>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

namespace BlamBootstrapper {

struct Expr {
    virtual ~Expr() = default;
    virtual llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) = 0;
};

struct NumberExpr : Expr {
    double value;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

enum class StmtType {
    PUB = 0,
    SCOPED,
    EXPR,
    FUNC_DECL,
    CONST_DECL,
    VAR_DECL,
    RETURN
};

struct Stmt {
    StmtType stmtType;
    virtual ~Stmt() = default; 
};

struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;
};

struct PubStmt : Stmt {
    std::unique_ptr<Stmt> body;
};

struct ScopedStmt : Stmt {
    std::string scope;
    std::vector<std::unique_ptr<Stmt>> body;
};

struct FuncDeclStmt : Stmt {
    std::string name;
    std::unique_ptr<ScopedStmt> body;
};

struct ConstDeclStmt : Stmt {
    std::string type;
    std::string name;
    std::unique_ptr<Stmt> value;
};

struct VarDeclStmt : Stmt {
    std::string type;
    std::string name;
    std::unique_ptr<Stmt> value;
};

struct ReturnStmt : Stmt {
    std::unique_ptr<Stmt> value;
};

void printAST(const std::unique_ptr<Stmt>& stmt, int indent);

/*
struct VariableExpr : Expr {
    std::string name;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct BinaryExpr : Expr {
    std::unique_ptr<Expr> op, lhs, rhs;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};
*/

}  // namespace BlamBoostrapper
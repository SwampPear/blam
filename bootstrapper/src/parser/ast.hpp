#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <map>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

namespace BlamBootstrapper {

inline std::map<std::string, llvm::Value*> namedValues;

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
    virtual llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) = 0;
};

struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct ScopedStmt : Stmt {
    std::string scope;
    std::vector<std::unique_ptr<Stmt>> body;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct PubStmt : Stmt {
    std::unique_ptr<Stmt> body;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct FuncDeclStmt : Stmt {
    std::string type;
    std::string name;
    std::unique_ptr<ScopedStmt> body;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct ConstDeclStmt : Stmt {
    std::string type;
    std::string name;
    std::unique_ptr<Stmt> value;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct VarDeclStmt : Stmt {
    std::string type;
    std::string name;
    std::unique_ptr<Stmt> value;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct ReturnStmt : Stmt {
    std::unique_ptr<Stmt> value;
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
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
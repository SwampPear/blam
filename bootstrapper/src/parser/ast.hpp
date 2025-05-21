#pragma once
#include <memory>
#include <string>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

namespace Parser {

struct Expr {
    virtual ~Expr() = default;
    virtual llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) = 0;
};

struct NumberExpr : Expr {
    double value;
    NumberExpr(double v) : value(v) {}
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct BinaryExpr : Expr {
    char op;
    std::unique_ptr<Expr> lhs, rhs;
    BinaryExpr(char o, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
        : op(o), lhs(std::move(l)), rhs(std::move(r)) {}
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct Stmt { virtual ~Stmt() = default; };

struct FunctionDecl : Stmt {
    std::string name;
    std::vector<std::unique_ptr<Stmt>> body;
};

struct VarDecl : Stmt {
    std::string type;
    std::string name;
    std::unique_ptr<Expr> init;
};

struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;
};

}  // namespace Parser
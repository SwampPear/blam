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

    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct VariableExpr : Expr {
    std::string name;

    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct BinaryExpr : Expr {
    std::unique_ptr<Expr> op, lhs, rhs;

    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct Stmt {
    virtual ~Stmt() = default; 
};

enum class ScopeType {
    FUNC,
    IF,
    EL
};

struct ScopedStmt : Stmt {
    ScopeType type;
    std::vector<std::unique_ptr<Stmt>> body;
};

struct ProgramStmt : Stmt {
    std::vector<std::unique_ptr<ScopedStmt>> body;
};

struct FuncDeclStmt : Stmt {
    std::string name;
    std::unique_ptr<ScopedStmt> body;
};

struct VarDeclStmt : Stmt {
    std::string type;
    std::string name;
    std::unique_ptr<Stmt> init;
};

struct ReturnStmt : Stmt {
    std::unique_ptr<Stmt> value;
};

/*
struct CallExpr : Expr {
    std::string callee;
    std::vector<pExpr> args;
    CallExpr(const std::string& c, std::vector<pExpr> a) : callee(c), args(std::move(a)) {}
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct BoolExpr : Expr {
    bool value;
    BoolExpr(bool v) : value(v) {}
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

struct IfExpr : Expr {
    pExpr cond, thenBranch, elseBranch;
    IfExpr(pExpr c, pExpr t, pExpr e) : cond(std::move(c)), thenBranch(std::move(t)), elseBranch(std::move(e)) {}
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};
*/

}  // namespace Parser
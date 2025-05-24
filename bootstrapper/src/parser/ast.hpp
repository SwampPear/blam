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

using pExpr = std::unique_ptr<Expr>;


struct NumberExpr : Expr {
    double value;

    NumberExpr(double v) : value(v) {}
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};


struct BinaryExpr : Expr {
    pExpr op, lhs, rhs;

    BinaryExpr(pExpr o, pExpr l, pExpr r) : 
        op(std::move(o)), lhs(std::move(l)), rhs(std::move(r)) {}

    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};


struct Stmt { 
    virtual ~Stmt() = default; 
};

using pStmt = std::unique_ptr<Stmt>;


struct FunctionDecl : Stmt {
    std::string name;
    std::vector<pStmt> body;
};


struct VarDecl : Stmt {
    std::string type;
    std::string name;
    pExpr init;
};


}  // namespace Parser
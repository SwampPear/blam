#pragma once
#include <memory>
#include <string>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

namespace Parser {

/**
 * @brief Base class for all expression nodes in AST.
 */
struct Expr {
    virtual ~Expr() = default;

    /**
     * @brief Generates corresponding LLVM IR code for the expression.
     * 
     * @param ctx LLVM context where the code is generated.
     * @param builder IRBuilder used to construct the LLVM instructions.
     * @return A pointer to the generated LLVM Value representing the expression.
     */
    virtual llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) = 0;
};

using pExpr = std::unique_ptr<Expr>;

/**
 * @brief Numeric constant.
 */
struct NumberExpr : Expr {
    double value;

    NumberExpr(double v) : value(v) {}

    /**
     * @brief Generates corresponding LLVM IR code for the expression.
     * 
     * @param ctx LLVM context where the code is generated.
     * @param builder IRBuilder used to construct the LLVM instructions.
     * @return A pointer to the generated LLVM Value representing the expression.
     */
    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

/**
 * @brief Binary expression with operator and two operands.
 */
struct BinaryExpr : Expr {
    pExpr op, lhs, rhs;

    BinaryExpr(pExpr o, pExpr l, pExpr r) : 
        op(std::move(o)), lhs(std::move(l)), rhs(std::move(r)) {}

    llvm::Value* codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) override;
};

/**
 * @brief Base class for all statement types in AST.
 */
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
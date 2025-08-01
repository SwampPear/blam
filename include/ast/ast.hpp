// ast.hpp
#pragma once
#include <memory>
#include <string>
#include <vector>

namespace Blam {

struct ASTNode {
    virtual ~ASTNode() = default;
};

struct Expr : ASTNode {};

struct NumberExpr : Expr {
    double value;
    explicit NumberExpr(double val) : value(val) {}
};

struct VariableExpr : Expr {
    std::string name;
    explicit VariableExpr(const std::string& n) : name(n) {}
};

struct BinaryExpr : Expr {
    std::string op;
    std::shared_ptr<Expr> lhs;
    std::shared_ptr<Expr> rhs;
    BinaryExpr(std::string op, std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs)
        : op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
};

struct CallExpr : Expr {
    std::string callee;
    std::vector<std::shared_ptr<Expr>> args;
    CallExpr(std::string callee, std::vector<std::shared_ptr<Expr>> args)
        : callee(std::move(callee)), args(std::move(args)) {}
};

}  // namespace Blam

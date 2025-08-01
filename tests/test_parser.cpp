#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include <typeinfo>
#include <iostream>

#include "parser/parser.hpp"
#include "tokenizer/tokenizer.hpp"
#include "ast/ast.hpp"

using namespace Blam;

template <typename T, typename Base>
bool isa(const std::shared_ptr<Base>& ptr) {
    return std::dynamic_pointer_cast<T>(ptr) != nullptr;
}

template <typename T, typename Base>
std::shared_ptr<T> cast(const std::shared_ptr<Base>& ptr) {
    return std::dynamic_pointer_cast<T>(ptr);
}

TEST_CASE("Parser handles simple number", "[parser]") {
    std::string input = "42";
    auto tokens = tokenize(input);
    Parser parser(tokens->head, input);
    auto expr = parser.parse();

    REQUIRE(isa<NumberExpr>(expr[0]));
    REQUIRE(cast<NumberExpr>(expr[0])->value == 42);
}

TEST_CASE("Parser handles variable identifier", "[parser]") {
    std::string input = "x";
    auto tokens = tokenize(input);
    Parser parser(tokens->head, input);
    auto expr = parser.parse();

    REQUIRE(isa<VariableExpr>(expr[0]));
    REQUIRE(cast<VariableExpr>(expr[0])->name == "x");
}

TEST_CASE("Parser handles binary expression", "[parser]") {
    std::string input = "x + 3";
    auto tokens = tokenize(input);
    Parser parser(tokens->head, input);
    auto expr = parser.parse();

    REQUIRE(isa<BinaryExpr>(expr[0]));
    auto bin = cast<BinaryExpr>(expr[0]);
    REQUIRE(bin->op == "+");
    REQUIRE(isa<VariableExpr>(bin->lhs));
    REQUIRE(isa<NumberExpr>(bin->rhs));
    REQUIRE(cast<VariableExpr>(bin->lhs)->name == "x");
    REQUIRE(cast<NumberExpr>(bin->rhs)->value == 3.0);
}

TEST_CASE("Parser handles compound expression", "[parser]") {
    std::string input = "a + b - 2";
    auto tokens = tokenize(input);
    Parser parser(tokens->head, input);
    auto expr = parser.parseExpression();

    REQUIRE(isa<BinaryExpr>(expr));
    auto sub = cast<BinaryExpr>(expr);
    REQUIRE(sub->op == "-");

    // LHS of '-' should be a + b
    REQUIRE(isa<BinaryExpr>(sub->lhs));
    auto add = cast<BinaryExpr>(sub->lhs);
    REQUIRE(add->op == "+");
    REQUIRE(isa<VariableExpr>(add->lhs));
    REQUIRE(cast<VariableExpr>(add->lhs)->name == "a");
    REQUIRE(isa<VariableExpr>(add->rhs));
    REQUIRE(cast<VariableExpr>(add->rhs)->name == "b");

    // RHS of '-' should be 2
    REQUIRE(isa<NumberExpr>(sub->rhs));
    REQUIRE(cast<NumberExpr>(sub->rhs)->value == 2.0);
}

TEST_CASE("Parser respects operator precedence", "[parser]") {
    std::string input = "a + b * 2";
    auto tokens = tokenize(input);
    Parser parser(tokens->head, input);
    auto expr = parser.parseExpression();

    std::cout << "expr type: " << typeid(*expr).name() << '\n';
    REQUIRE(isa<BinaryExpr>(expr));
    auto add = cast<BinaryExpr>(expr);
    REQUIRE(add->op == "+");

    // LHS of '+' should be 'a'
    REQUIRE(isa<VariableExpr>(add->lhs));
    REQUIRE(cast<VariableExpr>(add->lhs)->name == "a");

    // RHS of '+' should be 'b * 2'
    REQUIRE(isa<BinaryExpr>(add->rhs));
    auto mult = cast<BinaryExpr>(add->rhs);
    REQUIRE(mult->op == "*");

    // Check 'b' and '2'
    REQUIRE(isa<VariableExpr>(mult->lhs));
    REQUIRE(cast<VariableExpr>(mult->lhs)->name == "b");

    REQUIRE(isa<NumberExpr>(mult->rhs));
    REQUIRE(cast<NumberExpr>(mult->rhs)->value == 2.0);
}

TEST_CASE("Parser respects operator precedence when lower-precedence operator comes second", "[parser]") {
    std::string input = "a * b + 2";
    auto tokens = tokenize(input);
    Parser parser(tokens->head, input);
    auto expr = parser.parseExpression();

    REQUIRE(isa<BinaryExpr>(expr));
    auto add = cast<BinaryExpr>(expr);
    REQUIRE(add->op == "+");

    // LHS of '+' should be 'a * b'
    REQUIRE(isa<BinaryExpr>(add->lhs));
    auto mult = cast<BinaryExpr>(add->lhs);
    REQUIRE(mult->op == "*");

    REQUIRE(isa<VariableExpr>(mult->lhs));
    REQUIRE(cast<VariableExpr>(mult->lhs)->name == "a");

    REQUIRE(isa<VariableExpr>(mult->rhs));
    REQUIRE(cast<VariableExpr>(mult->rhs)->name == "b");

    // RHS of '+' should be 2
    REQUIRE(isa<NumberExpr>(add->rhs));
    REQUIRE(cast<NumberExpr>(add->rhs)->value == 2.0);
}


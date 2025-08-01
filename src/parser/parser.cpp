#include <stdexcept>
#include <string>
#include <iostream>
#include "parser/parser.hpp"

using namespace Blam;

/* ------------------------------------------------- helpers */

Parser::Parser(std::shared_ptr<LListNode<Token>> head, const std::string& src)
    : curr(std::move(head)), src(src) {}

Token& Parser::peek() {
    if (!curr || !curr->data) {
        throw std::runtime_error("Unexpected end of token stream.");
    }
    return *curr->data;
}

void Parser::skipWhitespace() {
    while (curr && curr->data &&
           (curr->data->type == Type::WHITESPACE || curr->data->type == Type::RAW))
        curr = curr->next;
}

void Parser::advance() {
    if (curr) curr = curr->next;
    skipWhitespace();
}

bool Parser::match(Type t) {
    skipWhitespace();
    if (curr && curr->data->type == t) {
        advance();
        return true;
    }
    return false;
}

/* ------------------------------------------------- precedence + associativity */

int Parser::getPrecedence(Type type) {
    switch (type) {
        case Type::EQEQ:
        case Type::NEQ:                      return  5;
        case Type::LT:
        case Type::LTE:
        case Type::GT:
        case Type::GTE:                      return 10;
        case Type::PLUS:
        case Type::MIN:                      return 20;
        case Type::MULT:
        case Type::DIV:                      return 30;
        case Type::EXP:                      return 40;
        default:                             return -1;
    }
}

bool Parser::isRightAssociative(Type type) {
    return type == Type::EXP;
}

/* ------------------------------------------------- recursive-descent parser */

std::shared_ptr<Expr> Parser::parseExpression() {
    auto lhs = parsePrimary();
    return parseBinaryOpRHS(0, lhs);
}

std::shared_ptr<Expr> Parser::parsePrimary() {
    if (!curr || !curr->data)
        throw std::runtime_error("Unexpected end of input to primary expression.");

    Token& tok = peek();

    if (tok.type == Type::IDENT) {
        std::string name = src.substr(tok.pos, tok.len);
        advance();
        return std::make_shared<VariableExpr>(name);
    }

    if (tok.type == Type::NUMBER) {
        double value = std::stod(src.substr(tok.pos, tok.len));
        advance();
        return std::make_shared<NumberExpr>(value);
    }

    if (tok.type == Type::SMBRACKET_L) {
        advance(); // consume '('
        auto expr = parseExpression();
        if (!match(Type::SMBRACKET_R))
            throw std::runtime_error("Expected closing ')'");
        return expr;
    }

    throw std::runtime_error("Unknown token in expression: " + typeToString(tok.type));
}

std::shared_ptr<Expr> Parser::parseBinaryOpRHS(int exprPrec, std::shared_ptr<Expr> lhs) {
    while (true) {
        if (!curr || !curr->data) return lhs;

        Type opType = curr->data->type;
        int opPrec = getPrecedence(opType);

        if (opPrec < exprPrec) return lhs;

        std::string opSymbol = src.substr(curr->data->pos, curr->data->len);
        advance(); // consume operator

        auto rhs = parsePrimary();  // parse next atom, don't recurse yet

        int nextPrec = getPrecedence(curr && curr->data ? curr->data->type : Type::END);
        if (opPrec < nextPrec || (opPrec == nextPrec && isRightAssociative(opType))) {
            rhs = parseBinaryOpRHS(opPrec + (isRightAssociative(opType) ? 0 : 1), rhs);
        }

        lhs = std::make_shared<BinaryExpr>(opSymbol, lhs, rhs);
    }
}

/* ------------------------------------------------- top-level */

std::vector<std::shared_ptr<Expr>> Parser::parse() {
    std::vector<std::shared_ptr<Expr>> ast;
    skipWhitespace();
    while (curr && curr->data && curr->data->type != Type::END) {
        ast.push_back(parseExpression());
    }
    return ast;
}

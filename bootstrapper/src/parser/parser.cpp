#include <stdexcept>
#include <stack>
#include <iostream>

#include "parser/parser.hpp"
#include "parser/ast.hpp"
#include "tokenizer/token.hpp"

namespace BlamBootstrapper {

void expect(const std::vector<Token>& tokens, size_t& index, Type expected) {
    if (index >= tokens.size()) {
        throw std::runtime_error("Compiler Error: Index out of bounds.");
    }

    if (tokens[index].type != expected) {
        throw std::runtime_error("Compiler Error: Unexpected token type matching: " + 
            std::string(TOKEN_EXPR[tokens[index].type]));
    }
}

std::string extractString(const Token& tok, const std::string& input) {
    return input.substr(tok.pos, tok.len);
}

bool shouldSkip(const std::vector<Token>& tokens, size_t& index) {
    return tokens[index].type == Type::MLINE_COMMENT ||
           tokens[index].type == Type::SLINE_COMMENT ||
           tokens[index].type == Type::WHITESPACE ||
           tokens[index].type == Type::NLINE;
}

void skip(const std::vector<Token>& tokens, size_t& index) {
    while (index < tokens.size() && shouldSkip(tokens, index)) {
        ++index;
    }
}

bool match(const std::vector<Token>& tokens, size_t index, std::vector<Type> pattern) {
    if (index >= tokens.size()) {
        throw std::runtime_error("Compiler Error: Index out of bounds.");
    }

    size_t i = index;
    for (const auto& type : pattern) {
        if (i >= tokens.size()) {
            throw std::runtime_error("Compiler Error: Index out of bounds.");
        }

        if (type == Type::SKIP) {
            skip(tokens, i);
        } else if (tokens[i].type == type) {
            i += 1;
        } else {
            return false;
        }
    }

    return true;
}

std::unique_ptr<Stmt> processPub(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src) {
    std::cout << "Processing pub token" << std::endl;

    // scope check
    if (scope != "") {
        throw std::runtime_error("Compiler Error: 'pub' keyword only allowed in top-level declaration.");
    }

    // pub statement
    auto stmt = std::make_unique<PubStmt>();
    stmt->type = StmtType::PUB;
    index += 1;

    // skip whitespace
    skip(tokens, index);

    // process statement
    stmt->stmt = processToken(tokens, index, scope, src);

    // allowed statement types
    if (stmt->stmt->type != StmtType::FUNC_DECL &&
        stmt->stmt->type != StmtType::CONST_DECL) {
        throw std::runtime_error("Compiler Error: Expected function or constant declaration.");
    }

    return stmt;
}

std::unique_ptr<Stmt> processDef(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src) {
    std::cout << "Processing def token" << std::endl;

    // function declaration statement
    auto stmt = std::make_unique<FuncDeclStmt>();
    stmt->type = StmtType::FUNC_DECL;
    stmt->body = std::make_unique<ScopedStmt>();
    stmt->body->type = ScopeType::FUNC;
    index += 1;

    // skip whitespace
    skip(tokens, index);

    // check for text and extract function name
    expect(tokens, index, Type::TEXT);
    stmt->name = extractString(tokens[index], src);
    std::cout << "Processing function: " + stmt->name << std::endl;
    index += 1;

    // skip whitespace
    skip(tokens, index);

    // check or arglist TODO: later, implement actual arg lists
    expect(tokens, index, Type::SMBRACKET_L);
    index += 1;
    expect(tokens, index, Type::SMBRACKET_R);
    index += 1;

    // skip whitespace
    skip(tokens, index);

    // check for open bracket
    expect(tokens, index, Type::CUBRACKET_L);
    index += 1;

    // skip whitespace
    skip(tokens, index);

    // scan body
    while (index < tokens.size() && tokens[index].type != Type::CUBRACKET_R) {
        if (shouldSkip(tokens, index)) {
            ++index;
            continue;
        }

        stmt->body->body.push_back(processToken(tokens, index, stmt->name, src));
    }

    // skip the right bracket
    index += 1;

    return stmt;
}

std::unique_ptr<Stmt> processRet(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src) {
    std::cout << "Processing ret token" << std::endl;

    // function declaration statement
    auto stmt = std::make_unique<ReturnStmt>();
    stmt->type = StmtType::RETURN;
    index += 1;

    // skip whitespace
    skip(tokens, index);
    
    // parse value
    stmt->value = processExpression(tokens, index, scope, src, Type::NLINE);
    index += 1;

    return stmt;
}

std::unique_ptr<Stmt> processText(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src) {
    std::cout << "Processing text token" << std::endl;
    std::cout << extractString(tokens[index], src);
    std::cout << "Processing text token" << std::endl;

    if (match(tokens, index, {Type::TEXT, Type::SKIP, Type::TEXT, Type::SKIP, Type::EQ})) {
        std::cout << "Processing variable declaration" << std::endl;

        // type
        auto stmt = std::make_unique<VarDeclStmt>();
        stmt->type = StmtType::VAR_DECL;
        stmt->varType = extractString(tokens[index], src);
        index += 1;

        // skip whitespace
        skip(tokens, index);

        // expect another text part and extract name
        stmt->varName = extractString(tokens[index], src);
        index += 1;
        
        // skip whitespace, =, whitespace
        skip(tokens, index);
        index += 1;
        skip(tokens, index);

        // process statement
        stmt->varStmt = processExpression(tokens, index, scope, src, Type::NLINE);

        // allowed statement types
        if (stmt->varStmt->type != StmtType::EXPR) {
            throw std::runtime_error("Compiler Error: Expected expression.");
        }

        index += 1;

        return stmt;
    } else {
        throw std::runtime_error("Unexpected pattern");
    }
}

std::unique_ptr<Stmt> processExpression(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src, Type delimeter) {
    std::cout << "Processing expression" << std::endl;
    // TODO: replace with routed algorithm that first detects what type of
    // expression is being handled, then uses shunting yard for arithmetic
    // and binary expressions and list joining for lists

    // but for now only parse number and arithmetic
    auto stmt = std::make_unique<ExprStmt>();
    stmt->type = StmtType::EXPR;

    auto expr = std::make_unique<NumberExpr>();
    expr->value = std::stod(extractString(tokens[index], src));

    stmt->expr = std::move(expr);

    return stmt;
}

std::unique_ptr<Stmt> processToken(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src) {
    switch (tokens[index].type) {
        case Type::PUB: {
            return processPub(tokens, index, scope, src);
        }
        case Type::DEF: {
            return processDef(tokens, index, scope, src);
        }
        case Type::RET: {
            return processRet(tokens, index, scope, src);
        }
        case Type::TEXT: {
            return processText(tokens, index, scope, src);
        }
        default:
            throw std::runtime_error("Compiler Error: Unexpected token type matching: " + 
                std::string(TOKEN_EXPR[tokens[index].type]));
    }
}

std::unique_ptr<ScopedStmt> parseProgram(const std::vector<Token>& tokens, const std::string& src) {
    auto prog = std::make_unique<ScopedStmt>();
    prog->type = ScopeType::ROOT;
    prog->body = std::vector<std::unique_ptr<Stmt>>();

    size_t index = 0;
    while (index < tokens.size()) {
        if (shouldSkip(tokens, index)) {
            ++index;
            continue;
        }

        prog->body.push_back(processToken(tokens, index, "", src));
    }

    return prog;
}

/*
pExpr parsePrimary(const std::vector<Token>& tokens, size_t& index) {
    const Token& tok = tokens[index++];
    return std::make_unique<Parser::NumberExpr>(1.0); // placeholder
}

pExpr parseExpression(const std::vector<Token>& tokens, size_t& index) {
    auto lhs = parsePrimary(tokens, index);
    while (index < tokens.size() &&
           (tokens[index].type == Type::PLUS || tokens[index].type == Type::MINUS ||
            tokens[index].type == Type::MULT || tokens[index].type == Type::DIV)) {

        char op = extractString(tokens[index])[0];
        ++index;
        auto rhs = parsePrimary(tokens, index);
        lhs = std::make_unique<Parser::BinaryExpr>(op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

std::unique_ptr<Parser::Stmt> parseStatement(const std::vector<Token>& tokens, size_t& index) {
    if (tokens[index].type == Type::KEYWORD && extractString(tokens[index]) == "print") {
        ++index;
        expect(tokens, index, Type::SMBRACKET_L);
        auto expr = parseExpression(tokens, index);
        expect(tokens, index, Type::SMBRACKET_R);
        return std::make_unique<Parser::ExprParser::Stmt>(std::move(expr));
    }
    if (tokens[index].type == Type::RAW) {
        std::string type = extractString(tokens[index++]);
        std::string name = extractString(tokens[index++]);
        expect(tokens, index, Type::EQ);
        auto expr = parseExpression(tokens, index);
        return std::make_unique<VarDecl>(type, name, std::move(expr));
    }
    throw std::runtime_error("Unknown statement");
}

std::unique_ptr<FunctionDecl> parseFunction(const std::vector<Token>& tokens, size_t& index) {
    ++index; // skip 'def'
    std::string name = extractString(tokens[index++]);
    expect(tokens, index, Type::SMBRACKET_L);
    expect(tokens, index, Type::SMBRACKET_R);
    expect(tokens, index, Type::CUBRACKET_L);

    auto fn = std::make_unique<FunctionDecl>();
    fn->name = name;
    while (tokens[index].type != Type::CUBRACKET_R) {
        fn->body.push_back(parseStatement(tokens, index));
    }
    ++index;
    return fn;
}
*/

}  // namespace BlamBootstrapper
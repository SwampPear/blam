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
    std::cout << "Processing pubadsfasd token" << std::endl;
    stmt->stmt = std::move(processToken(tokens, index, scope, src));

    std::cout << static_cast<int>(stmt->stmt->type) << std::endl;

    // allowed statement types
    if (stmt->stmt->type != StmtType::FUNC_DECL &&
        stmt->stmt->type != StmtType::CONST_DECL) {
        throw std::runtime_error("Compiler Error: Expected function or constant declaration.");
    }

    std::cout << "Processing pub token" << std::endl;

    return stmt;
}

std::unique_ptr<Stmt> processDef(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src) {
    std::cout << "Processing def token" << std::endl;

    // function declaration statement
    auto stmt = std::make_unique<FuncDeclStmt>();
    stmt->type = StmtType::FUNC_DECL;
    index += 1;

    std::cout << "Processing def token" << std::endl;

    // skip whitespace
    skip(tokens, index);

    std::cout << "Processing def token" << std::endl;

    // check for text and extract function name
    expect(tokens, index, Type::TEXT);
    std::string name = extractString(tokens[index], src);
    std::cout << "parsing function: " + name << std::endl;


    std::cout << "Processing def token" << std::endl;

    // process new scope


    return stmt;
}

std::unique_ptr<Stmt> processToken(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src) {
    switch (tokens[index].type) {
        case Type::PUB: {
            return processPub(tokens, index, scope, src);
        }
        case Type::DEF: {
            auto p = processDef(tokens, index, scope, src);
            std::cout << "asdf" << std::endl;
            return p;
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

        auto stmt = processToken(tokens, index, "", src);

        std::cout << "Parsed statement: " << typeid(*stmt).name() << std::endl;
        prog->body.push_back(std::move(stmt));
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
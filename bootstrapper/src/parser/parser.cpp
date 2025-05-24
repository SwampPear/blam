#include <stdexcept>

#include "parser.hpp"
#include "parser/ast.hpp"
#include "tokenizer/token.hpp"
#include <stack>


namespace Parser {

static bool expect(const std::vector<Tokenizer::Token>& tokens, size_t& index, Tokenizer::Type expected) {
    if (index >= tokens.size()) throw std::runtime_error("Index out of bounds");

    if (tokens[index].type != expected)
        return false;
        
    ++index;
    return true;
}

std::string extractString(const Tokenizer::Token& tok, const std::string& input) {
    return input.substr(tok.pos, tok.len);
}

bool shouldSkip(const std::vector<Tokenizer::Token>& tokens, size_t& index) {
    return tokens[index].type == Tokenizer::Type::MLINE_COMMENT ||
           tokens[index].type == Tokenizer::Type::SLINE_COMMENT ||
           tokens[index].type == Tokenizer::Type::WHITESPACE ||
           tokens[index].type == Tokenizer::Type::NLINE;
}

std::unique_ptr<Stmt> processPub(const std::vector<Tokenizer::Token>& tokens, size_t& index, std::string scope) {
    if (scope != "") {
        throw std::runtime_error("Compiler Error: 'pub' keyword only allowed in top-level function or constant declaration.");
    }

    auto stmt = std::make_unique<PubStmt>();
    stmt->type = StmtType::PUB;

    auto processedStmt = processToken(tokens, ++index, scope);
    if (processedStmt->type != StmtType::FUNC_DECL ||
        processedStmt->type != StmtType::CONST_DECL) {
        throw std::runtime_error("Compiler Error: Expected function or constant declaration.");
    }

    stmt->stmt = std::move(processedStmt);

    return stmt;
}

std::unique_ptr<Stmt> processToken(const std::vector<Tokenizer::Token>& tokens, size_t& index, std::string scope) {
    switch (tokens[index].type) {
        case Tokenizer::Type::PUB: {
            return processPub(tokens, index, scope);
        }
        default:
            throw std::runtime_error("Unexpected token type");
    }
}

std::unique_ptr<ScopedStmt> parseProgram(const std::vector<Tokenizer::Token>& tokens) {
    auto prog = std::make_unique<ScopedStmt>();
    prog->type = ScopeType::ROOT;
    prog->body = std::vector<std::unique_ptr<Stmt>>();

    size_t index = 0;
    while (index < tokens.size()) {
        if (shouldSkip(tokens, index)) {
            ++index;
            continue;
        }

        auto stmt = processToken(tokens, index, "");
        prog->body.push_back(std::move(stmt));
    }

    return prog;
}

/*
pExpr parsePrimary(const std::vector<Tokenizer::Token>& tokens, size_t& index) {
    const Tokenizer::Token& tok = tokens[index++];
    return std::make_unique<Parser::NumberExpr>(1.0); // placeholder
}

pExpr parseExpression(const std::vector<Tokenizer::Token>& tokens, size_t& index) {
    auto lhs = parsePrimary(tokens, index);
    while (index < tokens.size() &&
           (tokens[index].type == Tokenizer::Type::PLUS || tokens[index].type == Tokenizer::Type::MINUS ||
            tokens[index].type == Tokenizer::Type::MULT || tokens[index].type == Tokenizer::Type::DIV)) {

        char op = extractString(tokens[index])[0];
        ++index;
        auto rhs = parsePrimary(tokens, index);
        lhs = std::make_unique<Parser::BinaryExpr>(op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

std::unique_ptr<Parser::Stmt> parseStatement(const std::vector<Tokenizer::Token>& tokens, size_t& index) {
    if (tokens[index].type == Tokenizer::Type::KEYWORD && extractString(tokens[index]) == "print") {
        ++index;
        expect(tokens, index, Tokenizer::Type::SMBRACKET_L);
        auto expr = parseExpression(tokens, index);
        expect(tokens, index, Tokenizer::Type::SMBRACKET_R);
        return std::make_unique<Parser::ExprParser::Stmt>(std::move(expr));
    }
    if (tokens[index].type == Tokenizer::Type::RAW) {
        std::string type = extractString(tokens[index++]);
        std::string name = extractString(tokens[index++]);
        expect(tokens, index, Tokenizer::Type::EQ);
        auto expr = parseExpression(tokens, index);
        return std::make_unique<VarDecl>(type, name, std::move(expr));
    }
    throw std::runtime_error("Unknown statement");
}

std::unique_ptr<FunctionDecl> parseFunction(const std::vector<Tokenizer::Token>& tokens, size_t& index) {
    ++index; // skip 'def'
    std::string name = extractString(tokens[index++]);
    expect(tokens, index, Tokenizer::Type::SMBRACKET_L);
    expect(tokens, index, Tokenizer::Type::SMBRACKET_R);
    expect(tokens, index, Tokenizer::Type::CUBRACKET_L);

    auto fn = std::make_unique<FunctionDecl>();
    fn->name = name;
    while (tokens[index].type != Tokenizer::Type::CUBRACKET_R) {
        fn->body.push_back(parseStatement(tokens, index));
    }
    ++index;
    return fn;
}
*/

}  // namespace Parser
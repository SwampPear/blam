#include <stdexcept>

#include "parser.hpp"
#include "parser/ast.hpp"
#include "tokenizer/token.hpp"


namespace Parser {


static bool expect(const std::vector<Tokenizer::Token>& tokens, size_t& index, Tokenizer::Type expected) {
    if (tokens[index].type != expected)
        return false;
        
    ++index;
    return true;
}


std::string extractString(const Tokenizer::Token& tok, const std::string& input) {
    return input.substr(tok.pos, tok.len);
}


bool shouldSkip(const std::vector<Tokenizer::Token>& tokens, size_t& index) {
    return index < tokens.size() &&
            tokens[index].type == Tokenizer::Type::SLINE_COMMENT ||
           (tokens[index].type == Tokenizer::Type::SLINE_COMMENT ||
            tokens[index].type == Tokenizer::Type::WHITESPACE ||
            tokens[index].type == Tokenizer::Type::NLINE);
}

/*
pExpr parseNumber(const std::vector<Tokenizer::Token>& tokens, size_t& index) {
    const Tokenizer::Token& tok = tokens[index++];
    return std::make_unique<Parser::NumberExpr>(1.0); // placeholder
}*/

pStmt processToken(const std::vector<Tokenizer::Token>& tokens, size_t& index) {
    switch (tokens[index].type) {
        case Tokenizer::Type::NUMBER:
            // Handle number token
            break;
        default:
            throw std::runtime_error("Unexpected token type");
    }
}

std::vector<pStmt> parseProgram(const std::vector<Tokenizer::Token>& tokens) {
    std::vector<pStmt> prog;
    size_t index = 0;
    while (index < tokens.size()) {
        if (shouldSkip(tokens, index)) {
            ++index;
            continue;
        }

        pStmt stmt = processToken(tokens, index);
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
#pragma once
#include "tokenizer/token.hpp"
#include "parser/ast.hpp"

namespace Parser {
    
std::string extractString(const Tokenizer::Token& tok, const std::string& input);
static bool expect(const std::vector<Tokenizer::Token>& tokens, size_t& index, Tokenizer::Type expected);
std::unique_ptr<Parser::Expr> parsePrimary(const std::vector<Tokenizer::Token>& tokens, size_t& index);
std::unique_ptr<Parser::Expr> parseExpression(const std::vector<Tokenizer::Token>& tokens, size_t& index);
std::unique_ptr<Stmt> parseStatement(const std::vector<Tokenizer::Token>& tokens, size_t& index);
std::unique_ptr<FunctionDecl> parseFunction(const std::vector<Tokenizer::Token>& tokens, size_t& index);
std::vector<std::unique_ptr<Stmt>> parseProgram(const std::vector<Tokenizer::Token>& tokens);

}  // namespace Parser
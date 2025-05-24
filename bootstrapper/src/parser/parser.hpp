#pragma once

#include <string>
#include "tokenizer/token.hpp"
#include "parser/ast.hpp"

namespace Parser {

static bool expect(const std::vector<Tokenizer::Token>& tokens, size_t& index, Tokenizer::Type expected);
std::string extractString(const Tokenizer::Token& tok, const std::string& input);
bool shouldSkip(const std::vector<Tokenizer::Token>& tokens, size_t& index);

// std::unique_ptr<Parser::Expr> parseNumber(const std::vector<Tokenizer::Token>& tokens, size_t& index);
pStmt processToken(const std::vector<Tokenizer::Token>& tokens, size_t& index);
std::vector<std::unique_ptr<Stmt>> parseProgram(const std::vector<Tokenizer::Token>& tokens);

}  // namespace Parser
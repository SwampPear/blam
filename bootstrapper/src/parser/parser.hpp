#pragma once

#include <string>
#include <stack>
#include "tokenizer/token.hpp"
#include "parser/ast.hpp"

namespace Parser {

static bool expect(const std::vector<Tokenizer::Token>& tokens, size_t& index, Tokenizer::Type expected);
std::string extractString(const Tokenizer::Token& tok, const std::string& input);
bool shouldSkip(const std::vector<Tokenizer::Token>& tokens, size_t& index);

std::unique_ptr<Stmt> processPub(const std::vector<Tokenizer::Token>& tokens, size_t& index, std::string scope);
std::unique_ptr<Stmt> processToken(const std::vector<Tokenizer::Token>& tokens, size_t& index, std::string scope);

std::unique_ptr<ScopedStmt> parseProgram(const std::vector<Tokenizer::Token>& tokens);

}  // namespace Parser
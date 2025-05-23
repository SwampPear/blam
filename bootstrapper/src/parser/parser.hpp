#pragma once

#include <string>
#include "tokenizer/token.hpp"
#include "parser/ast.hpp"

namespace Parser {

/**
 * @brief Checks if the current token matches the type and advances the index.
 * 
 * @param tokens The list of tokens.
 * @param index The current index in the token list.
 * @param expected Expected token type.
 * @return true if the current token matches the expected type, false otherwise.
 */
static bool expect(const std::vector<Tokenizer::Token>& tokens, size_t& index, Tokenizer::Type expected);


std::string extractString(const Tokenizer::Token& tok, const std::string& input);

std::unique_ptr<Parser::Expr> parseNumber(const std::vector<Tokenizer::Token>& tokens, size_t& index);
std::vector<std::unique_ptr<Stmt>> parseProgram(const std::vector<Tokenizer::Token>& tokens);

}  // namespace Parser
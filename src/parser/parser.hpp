/*
#pragma once

#include <string>
#include <stack>
#include "tokenizer/token.hpp"
#include "parser/ast.hpp"

namespace Blam {

struct TokenExpr {
    bool single;
    Type type;
};

void expect(const std::vector<Token>& tokens, size_t& index, Type expected);
std::string extractString(const Token& tok, const std::string& input);

bool shouldSkip(const std::vector<Token>& tokens, size_t& index);
void skip(const std::vector<Token>& tokens, size_t& index);

std::unique_ptr<Stmt> processPub(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src);
std::unique_ptr<Stmt> processDef(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src);
std::unique_ptr<Stmt> processRet(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src);
std::unique_ptr<Stmt> processText(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src);

std::unique_ptr<Stmt> processExpression(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src, Type delimeter);
std::unique_ptr<Stmt> processToken(const std::vector<Token>& tokens, size_t& index, std::string scope, const std::string& src);

std::unique_ptr<Stmt> parseProgram(const std::vector<Token>& tokens, const std::string& src);

}  // namespace Blam
  */
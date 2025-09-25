#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <memory>

#include "tokenizer/token.hpp"

namespace BlamBootstrapper {

std::string toString(std::shared_ptr<Token> token, const std::string& src);
void printTokens(std::shared_ptr<Token> head, const std::string& src);
void printTokens(const std::vector<Token>& tokens, const std::string& src);

std::shared_ptr<Token> processRawToken(std::shared_ptr<Token> victim, Type type, const std::string& input);
std::shared_ptr<Token> tokenize(const std::string& src);
std::vector<Token> tokenizeFile(const std::string& fp);

}  // namespace BlamBootstrapper
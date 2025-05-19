#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <cctype>


namespace Blam {

enum class TokenType : uint8_t {
    Identifier = 0,
    Keyword,
    Literal,
    Operator,
    Separator,
    Comment,
    Whitespace
};

inline const char* toString(TokenType type) {
    switch (type) {
        case TokenType::Identifier: return "Identifier";
        case TokenType::Keyword: return "Keyword";
        case TokenType::Literal: return "Literal";
        case TokenType::Operator: return "Operator";
        case TokenType::Separator: return "Separator";
        case TokenType::Comment: return "Comment";
        case TokenType::Whitespace: return "Whitespace";
        default: return "Unknown";
    }
}

struct Token {
    const char* src;  // source string
    uint16_t pos;     // position
    uint16_t len;     // length
};


std::vector<Token> tokenize(const std::string& input);
std::vector<Token> tokenizeFile(const std::string& fp);


}  // namespace Token
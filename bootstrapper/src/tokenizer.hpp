#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <memory>  // for smart pointers

namespace Tokenizer {

/**
 * Token types in order of parsing step.
 */
enum class Type : uint8_t {
    // comments
    RAW = 0,             // raw uncategorized text
    MLINE_COMMENT,       // #* *#
    SLINE_COMMENT,       // # \n
    STRING,              // "<any char>" (multiline by default)
    WHITESPACE,          // \s+
    NLINE,               // \n (newline)
    SMBRACKET_L,         // (
    SMBRACKET_R,         // )
    SQBRACKET_L,         // [
    SQBRACKET_R,         // ]
    CUBRACKET_L,         // {
    CUBRACKET_R,         // }
    PLUS,                // +
    MINUS,               // -
    DIV,                 // /
    EXP,                 // **
    MULT,                // * 
    KEYWORD,             // def|ret|struct|enum
};

/**
 * Token parsing expressions.
 */
static std::unordered_map<Type, std::string> tokenExpression = {
    {Type::MLINE_COMMENT, ""}, //R"(\#\*[\s\S]*?\*\#)"},
    {Type::SLINE_COMMENT, R"(#([^\n]*)\n)"},
    {Type::STRING, ""},
    {Type::WHITESPACE, ""},
    {Type::NLINE, ""},
    {Type::SMBRACKET_L, ""},
    {Type::SMBRACKET_R, ""},
    {Type::SQBRACKET_L, ""},
    {Type::SQBRACKET_R, ""},
    {Type::CUBRACKET_L, ""},
    {Type::CUBRACKET_R, ""},
    {Type::PLUS, ""},
    {Type::MINUS, ""},
    {Type::DIV, ""},
    {Type::EXP, ""},
    {Type::MULT, ""},
    {Type::KEYWORD, ""}
};

struct Token {
    Type type;
    uint16_t pos;     // position
    uint16_t len;     // length
    std::shared_ptr<Token> next;
    std::shared_ptr<Token> prev;
};

std::shared_ptr<Token> tokenize(const std::string& input);
std::shared_ptr<Token> tokenizeFile(const std::string& fp);

}  // namespace Tokenizer
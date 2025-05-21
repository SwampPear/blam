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
    EQ,                  // =
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
    {Type::MLINE_COMMENT, ""},
    {Type::SLINE_COMMENT, R"(#([^\n]*)(\n|$))"},
    {Type::STRING, R"("(\\.|[^"\\])*"|'(\\.|[^'\\])*')"},
    {Type::WHITESPACE, R"([ \t]+)"},
    {Type::NLINE, R"(\r?\n)"},
    {Type::SMBRACKET_L, R"(\()"},
    {Type::SMBRACKET_R, R"(\))"},
    {Type::SQBRACKET_L, R"(\[)"},
    {Type::SQBRACKET_R, R"(\])"},
    {Type::CUBRACKET_L, R"(\{)"},
    {Type::CUBRACKET_R, R"(\})"},
    {Type::EQ, R"(=)"},
    {Type::PLUS, R"(\+)"},
    {Type::MINUS, R"(\-)"},
    {Type::DIV, R"(\/)"},
    {Type::EXP, R"(\^)"},
    {Type::MULT, R"(\*)"},
    {Type::KEYWORD, ""}
};

// R"(\b(if|else|for|while|return|let|fn|true|false|null)\b)"}

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
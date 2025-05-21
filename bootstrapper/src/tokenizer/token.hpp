#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>


namespace Tokenizer {

// ordered in lexeme hierarchy first to last
enum class Type : uint8_t {
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
    DEF,                 // def
    KEYWORD
};

struct Token {
    Type type;      // token type
    uint16_t pos;   // position in terms of source
    uint16_t len;   // length in terms of source
    std::shared_ptr<Token> prev = nullptr;
    std::shared_ptr<Token> next = nullptr;
};

static std::unordered_map<Type, std::string> TOKEN_EXPR = {
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
    {Type::DEF, R"(\bdef\b)"},
    {Type::KEYWORD, ""}
};

}  // Tokenizer
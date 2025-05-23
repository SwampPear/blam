#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>


namespace Tokenizer {

// ordered in lexeme hierarchy first to last
enum class Type : uint8_t {
    RAW = 0,             // raw uncategorized text

    // comments
    MLINE_COMMENT,       // #* *#
    SLINE_COMMENT,       // # \n

    // strings
    STRING,              // "example"

    // reserved keywords
    AND,                 // and
    OR,                  // or
    DEF,                 // def
    RET,                 // ret
    IF,                  // if
    EL,                  // el
    ELIF,                // elif
    WHILE,               // while
    FOR,                 // for
    IN,                  // in
    BREAK,               // break
    CONTINUE,            // continue
    TRUE,                // true
    FALSE,               // false
    NIL,                 // nil

    TEXT,                // any text

    // numbers
    DECIMAL,             // 0.234
    NUMBER,              // 123

    // space
    WHITESPACE,          // \s+
    NLINE,               // \n (newline)

    // delimeters
    SMBRACKET_L,         // (
    SMBRACKET_R,         // )
    SQBRACKET_L,         // [
    SQBRACKET_R,         // ]
    CUBRACKET_L,         // {
    CUBRACKET_R,         // }

    // operators
    ARROW,               // -> 
    BAND,                // &
    BOR,                 // |
    BXOR,                // ^
    BLS,                 // <<
    BRS,                 // >>
    DOT,                 // .  
    COMMA,               // ,
    COLON,               // :
    AT,                  // @

    // comparison
    EQEQ,                // ==
    NEQ,                 // !=
    LT,                  // <
    GT,                  // >
    LTE,                 // <=
    GTE,                 // >=

    // arithmetic
    EQ,                  // =
    PLUS,                // +
    MIN,                 // -
    DIV,                 // /
    EXP,                 // **
    MULT,                // * 
    PLUSEQ,              // +=
    MINEQ,               // -=
    DIVEQ,               // /=
    MULTEQ,              // *=
};

struct Token {
    Type type;      // token type
    uint16_t pos;   // position in terms of source
    uint16_t len;   // length in terms of source
    std::shared_ptr<Token> prev = nullptr;
    std::shared_ptr<Token> next = nullptr;
};

static std::unordered_map<Type, std::string> TOKEN_EXPR = {
    {Type::MLINE_COMMENT, R"(#\*[^*]*\*#)"},
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
    {Type::MIN, R"(\-)"},
    {Type::DIV, R"(\/)"},
    {Type::EXP, R"(\^)"},
    {Type::MULT, R"(\*)"},
    {Type::DEF, R"(\bdef\b)"},
    {Type::RET, R"(\bret\b)"},
};

/*
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
    {Type::RET, R"(\bret\b)"},
};*/

}  // namespace Tokenizer
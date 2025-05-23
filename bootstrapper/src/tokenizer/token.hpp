#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>


namespace Tok {

// ordered in lexeme hierarchy first to last
enum class Type : uint8_t {
    RAW = 0,             // raw uncategorized text

    // comments
    MLINE_COMMENT,       // #* *#
    SLINE_COMMENT,       // # \n

    // strings (should be parsed first to account for characters within)
    STRING,              // "<any char>" (multiline by default)

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

    // operators that need parsed before arithmetic oerators
    ARROW,               // -> 
    BAND,                // &
    BOR,                 // |
    BXOR,                // ^
    BLS,                 // <<
    BRS,                 // >>

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

    // comparison
    EQEQ,                // ==
    NEQ,                 // !=
    LT,                  // <
    GT,                  // >
    LTE,                 // <=
    GTE,                 // >=

    // decimal (should parse before because of dot)
    DECIMAL,             // 0.234

    // operators
    DOT,                 // .  
    COMMA,               // ,
    COLON,               // :
    AT,                  // @

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

    // text (any other text later determined to be a type, class, e.t.c.)
    TEXT,

    // number (parsed last because of numbers being in text)
    NUMBER,              // 123
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
    {Type::RET, R"(\bret\b)"},
};

}  // namespace Tok
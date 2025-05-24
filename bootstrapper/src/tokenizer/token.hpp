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
    NLINE,               // \n (newline)
    WHITESPACE,          // \s+
   

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
    PLUSEQ,              // +=
    MINEQ,               // -=
    DIVEQ,               // /=
    MULTEQ,              // *=
    EQ,                  // =
    PLUS,                // +
    MIN,                 // -
    DIV,                 // /
    EXP,                 // **
    MULT                 // * 
};

struct Token {
    Type type;      // token type
    uint16_t pos;   // position in terms of source
    uint16_t len;   // length in terms of source
    std::shared_ptr<Token> prev = nullptr;
    std::shared_ptr<Token> next = nullptr;
};

static std::unordered_map<Type, std::string> TOKEN_EXPR = {
    RAW = 0,             // raw uncategorized text
    {Type::MLINE_COMMENT, R"(#\*[^*]*\*#)"},
    {Type::SLINE_COMMENT, R"(#([^\n]*)(\n|$))"},
    {Type::STRING, R"("(\\.|[^"\\])*"|'(\\.|[^'\\])*')"},
    {Type::AND, R"(\band\b)"},
    {Type::OR, R"(\bor\b)"},
    {Type::DEF, R"(\bdef\b)"},
    {Type::RET, R"(\bret\b)"},
    {Type::IF, R"(\bif\b)"},
    {Type::EL, R"(\bel\b)"},
    {Type::ELIF, R"(\belif\b)"},
    {Type::WHILE, R"(\bwhile\b)"},
    {Type::FOR, R"(\bfor\b)"},
    {Type::IN, R"(\bin\b)"},
    {Type::BREAK, R"(\bbreak\b)"},
    {Type::CONTINUE, R"(\bcontinue\b)"},
    {Type::TRUE, R"(\btrue\b)"},
    {Type::FALSE, R"(\bfalse\b)"},
    {Type::NIL, R"(\bnil\b)"},
    {Type::TEXT, R"([a-zA-Z_][a-zA-Z0-9_]*)"},
    {Type::DECIMAL, R"((?:\d+\.\d*|\.\d+))"},
    {Type::NUMBER, R"(\d+)"},
    {Type::NLINE, R"(\n)"},
    {Type::WHITESPACE, R"(\s+)"},
    {Type::SMBRACKET_L, R"(\()"},
    {Type::SMBRACKET_R, R"(\))"},
    {Type::SQBRACKET_L, R"(\[)"},
    {Type::SQBRACKET_R, R"(\])"},
    {Type::CUBRACKET_L, R"(\{)"},
    {Type::CUBRACKET_R, R"(\})"},
    {Type::ARROW, R"(->)"},
    {Type::BAND, R"(&)"},
    {Type::BOR, R"(\|)"},
    {Type::BXOR, R"(\^)"},
    {Type::BLS, R"(<<)"},
    {Type::BRS, R"(>>)"},
    {Type::DOT, R"(\.)"},
    {Type::COMMA, R"(,)"},
    {Type::COLON, R"(:)"},
    {Type::AT, R"(@)"},
    {Type::EQEQ, R"(==)"},
    {Type::NEQ, R"(!=)"},
    {Type::LTE, R"(<=)"},
    {Type::GTE, R"(>=)"},
    {Type::LT, R"(<)"},
    {Type::GT, R"(>)"},
    {Type::PLUSEQ, R"(\+=)"},
    {Type::MINEQ, R"(-=)"},
    {Type::DIVEQ, R"(/=)"},
    {Type::MULTEQ, R"(\*=)"},
    {Type::EQ, R"(=)"},
    {Type::PLUS, R"(\+)"},
    {Type::MIN, R"(-)"},
    {Type::DIV, R"(/)"},
    {Type::EXP, R"(\*\*)"},
    {Type::MULT, R"(\*)"}
};

}  // namespace Tokenizer
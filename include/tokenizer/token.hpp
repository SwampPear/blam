#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <sstream>

namespace Blam {

// ordered in parsing hierarchy
enum class Type : int8_t {
    SKIP = -2,           // used in parser for skippable tokens
    END,                 // end of file marker
    RAW,                 // raw src

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
    PUB,                 // pub
    CONST,               // const
    LET,                 // let

    // any text
    IDENT,

    // numbers
    DECIMAL,             // 0.234
    NUMBER,              // 123

    // space
    NLINE,               // \n
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

static std::unordered_map<Type, std::string> TOKEN_EXPR = {
    {Type::MLINE_COMMENT, R"(#\*[\s\S]*?\*#)"},
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
    {Type::PUB, R"(\bpub\b)"},
    {Type::CONST, R"(\bconst\b)"},
    {Type::LET, R"(\blet\b)"},
    {Type::IDENT, R"([a-zA-Z_][a-zA-Z0-9_]*)"},
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

struct Token {
    Type type;    // token type
    size_t pos;   // position in terms of source
    size_t len;   // length in terms of source

    std::string toString(const std::string& src);
};

inline std::string Token::toString(const std::string& src) {
    std::ostringstream oss;
    oss << "Type: " << static_cast<int>(this->type)
        << ", Pos: " << this->pos
        << ", Len: " << this->len
        << ", Content: " << std::endl
        << src.substr(this->pos, this->len) << std::endl;

    return oss.str();
}

}  // namespace Blam
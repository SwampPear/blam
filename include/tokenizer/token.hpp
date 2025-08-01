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

inline std::string typeToString(Type type) {
    switch (type) {
        case Type::SKIP: return "SKIP";
        case Type::END: return "END";
        case Type::RAW: return "RAW";

        // comments
        case Type::MLINE_COMMENT: return "MLINE_COMMENT";
        case Type::SLINE_COMMENT: return "SLINE_COMMENT";

        // strings
        case Type::STRING: return "STRING";

        // reserved keywords
        case Type::AND: return "AND";
        case Type::OR: return "OR";
        case Type::DEF: return "DEF";
        case Type::RET: return "RET";
        case Type::IF: return "IF";
        case Type::EL: return "EL";
        case Type::ELIF: return "ELIF";
        case Type::WHILE: return "WHILE";
        case Type::FOR: return "FOR";
        case Type::IN: return "IN";
        case Type::BREAK: return "BREAK";
        case Type::CONTINUE: return "CONTINUE";
        case Type::TRUE: return "TRUE";
        case Type::FALSE: return "FALSE";
        case Type::NIL: return "NIL";
        case Type::PUB: return "PUB";
        case Type::CONST: return "CONST";
        case Type::LET: return "LET";

        // identifiers and numbers
        case Type::IDENT: return "IDENT";
        case Type::DECIMAL: return "DECIMAL";
        case Type::NUMBER: return "NUMBER";

        // space
        case Type::NLINE: return "NLINE";
        case Type::WHITESPACE: return "WHITESPACE";

        // delimiters
        case Type::SMBRACKET_L: return "SMBRACKET_L";
        case Type::SMBRACKET_R: return "SMBRACKET_R";
        case Type::SQBRACKET_L: return "SQBRACKET_L";
        case Type::SQBRACKET_R: return "SQBRACKET_R";
        case Type::CUBRACKET_L: return "CUBRACKET_L";
        case Type::CUBRACKET_R: return "CUBRACKET_R";

        // operators
        case Type::ARROW: return "ARROW";
        case Type::BAND: return "BAND";
        case Type::BOR: return "BOR";
        case Type::BXOR: return "BXOR";
        case Type::BLS: return "BLS";
        case Type::BRS: return "BRS";
        case Type::DOT: return "DOT";
        case Type::COMMA: return "COMMA";
        case Type::COLON: return "COLON";
        case Type::AT: return "AT";

        // comparison
        case Type::EQEQ: return "EQEQ";
        case Type::NEQ: return "NEQ";
        case Type::LT: return "LT";
        case Type::GT: return "GT";
        case Type::LTE: return "LTE";
        case Type::GTE: return "GTE";

        // arithmetic
        case Type::PLUSEQ: return "PLUSEQ";
        case Type::MINEQ: return "MINEQ";
        case Type::DIVEQ: return "DIVEQ";
        case Type::MULTEQ: return "MULTEQ";
        case Type::EQ: return "EQ";
        case Type::PLUS: return "PLUS";
        case Type::MIN: return "MIN";
        case Type::DIV: return "DIV";
        case Type::EXP: return "EXP";
        case Type::MULT: return "MULT";

        default: return "UNKNOWN";
    }
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
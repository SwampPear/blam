#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace blam {
  struct Pos { size_t index{0}, line{1}, col{1}; };
  struct Range { Pos start{}, end{}; };

  // lexical categories only (no precedence baked in)
  enum class Tok : int32_t {
    // structure
    EOF_, // synthetic end of file
    NL,   // newline '\n' (expressions are newline-terminated)
    Identifier,
    Int,    // 123
    Float,  // 12.34 or .5 or 5.
    String, // "..."
    Char,   // 'x'

    // keywords (from control flow, errors, types, structs)
    KwIf,
    KwElse,
    KwSw,
    KwCase,
    KwDefault,
    KwWhile,
    KwDo,
    KwFor,
    KwIn,
    KwTry,
    KwCatch,
    KwReturn,
    KwBreak,
    KwContinue,
    KwRaise,
    KwStruct,
    KwPub,
    KwConst,
    KwImport,
    KwAs,
    KwTrue,
    KwFalse,

    // comments (lexer usually skips, but expose if needed for tools)
    SlComment, // # ... (to end of line)
    MlComment, // #* ... *#

    // operators / punctuators
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Bang,
    Amp,
    Pipe,
    Eq,
    EqEq,
    BangEq,
    Lt,
    Lte,
    Gt,
    Gte,
    AndAnd,
    OrOr,
    PlusPlus,
    MinusMinus,
    Arrow, // ->
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Comma,
    Dot,
    Colon
  };

  struct Token {
    Tok kind{};
    std::string lexeme{};
    Range range{};
  };

  inline const char *to_string(Tok k) {
    switch (k) {
    case Tok::EOF_:
      return "EOF";
    case Tok::NL:
      return "NL";
    case Tok::Identifier:
      return "Identifier";
    case Tok::Int:
      return "Int";
    case Tok::Float:
      return "Float";
    case Tok::String:
      return "String";
    case Tok::Char:
      return "Char";
    case Tok::KwIf:
      return "if";
    case Tok::KwElse:
      return "else";
    case Tok::KwSw:
      return "sw";
    case Tok::KwCase:
      return "case";
    case Tok::KwDefault:
      return "default";
    case Tok::KwWhile:
      return "while";
    case Tok::KwDo:
      return "do";
    case Tok::KwFor:
      return "for";
    case Tok::KwIn:
      return "in";
    case Tok::KwTry:
      return "try";
    case Tok::KwCatch:
      return "catch";
    case Tok::KwReturn:
      return "return";
    case Tok::KwBreak:
      return "break";
    case Tok::KwContinue:
      return "continue";
    case Tok::KwRaise:
      return "raise";
    case Tok::KwStruct:
      return "struct";
    case Tok::KwPub:
      return "pub";
    case Tok::KwConst:
      return "const";
    case Tok::KwImport:
      return "import";
    case Tok::KwAs:
      return "as";
    case Tok::KwTrue:
      return "true";
    case Tok::KwFalse:
      return "false";
    case Tok::KwAny:
      return "any";
    case Tok::KwI8:
      return "i8";
    case Tok::KwI16:
      return "i16";
    case Tok::KwI32:
      return "i32";
    case Tok::KwI64:
      return "i64";
    case Tok::KwInt:
      return "int";
    case Tok::KwU8:
      return "u8";
    case Tok::KwU16:
      return "u16";
    case Tok::KwU32:
      return "u32";
    case Tok::KwU64:
      return "u64";
    case Tok::KwUint:
      return "uint";
    case Tok::KwBool:
      return "bool";
    case Tok::KwChar:
      return "char";
    case Tok::KwStr:
      return "str";
    case Tok::SlComment:
      return "SLINE_COMMENT";
    case Tok::MlComment:
      return "MLINE_COMMENT";
    case Tok::Plus:
      return "+";
    case Tok::Minus:
      return "-";
    case Tok::Star:
      return "*";
    case Tok::Slash:
      return "/";
    case Tok::Percent:
      return "%";
    case Tok::Bang:
      return "!";
    case Tok::Amp:
      return "&";
    case Tok::Pipe:
      return "|";
    case Tok::Eq:
      return "=";
    case Tok::EqEq:
      return "==";
    case Tok::BangEq:
      return "!=";
    case Tok::Lt:
      return "<";
    case Tok::Lte:
      return "<=";
    case Tok::Gt:
      return ">";
    case Tok::Gte:
      return ">=";
    case Tok::AndAnd:
      return "&&";
    case Tok::OrOr:
      return "||";
    case Tok::PlusPlus:
      return "++";
    case Tok::MinusMinus:
      return "--";
    case Tok::Arrow:
      return "->";
    case Tok::LParen:
      return "(";
    case Tok::RParen:
      return ")";
    case Tok::LBrace:
      return "{";
    case Tok::RBrace:
      return "}";
    case Tok::LBracket:
      return "[";
    case Tok::RBracket:
      return "]";
    case Tok::Comma:
      return ",";
    case Tok::Dot:
      return ".";
    case Tok::Colon:
      return ":";
    }
    return "?";
  }
} // namespace blam

#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace blam
{

  // source positions
  struct Pos
  {
    size_t index{0}, line{1}, col{1};
  };
  struct Range
  {
    Pos start{}, end{};
  };

  // lexical categories only (no precedence baked in)
  enum class Tok : int32_t
  {
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
    KwAny,

    // primitive type keywords
    KwI8,
    KwI16,
    KwI32,
    KwI64,
    KwInt,
    KwU8,
    KwU16,
    KwU32,
    KwU64,
    KwUint,
    KwBool,
    KwChar,
    KwStr,

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

  struct Token
  {
    Tok kind{};
    std::string lexeme{};
    Range range{};
  };

  inline const char *to_string(Tok k)
  {
    switch (k)
    {
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

  // ----- Regex specifications (as strings) -----
  // Use these with a "maximal munch" lexer. Order matters: prefer longer/multi-char patterns first.
  struct TokSpec
  {
    Tok kind;
    std::string_view pattern; // ECMAScript-like; compile to std::regex or custom engine
  };

  // Multi-char ops before single-char; comments/strings before identifiers.
  inline const std::vector<TokSpec> &token_specs()
  {
    static const std::vector<TokSpec> specs = {
        // comments
        {Tok::MlComment, R"(#\*[\s\S]*?\*#)"},
        {Tok::SlComment, R"(#([^\n]*))"},

        // whitespace-newline (lexer: emit NL for '\n', otherwise skip spaces/tabs)
        // handle NL outside regex table for simplicity

        // literals
        {Tok::String, R"("(\\.|[^"\\])*")"},
        {Tok::Char, R"('(?:\\.|[^'\\])')"},
        {Tok::Float, R"((?:\d+\.\d*|\.\d+))"},
        {Tok::Int, R"(\d+)"},

        // keywords/identifiers: match Identifier then downcase-lookup in keyword map
        {Tok::Identifier, R"([A-Za-z_][A-Za-z0-9_]*)"},

        // operators / punctuators
        {Tok::Arrow, R"(->)"},
        {Tok::PlusPlus, R"(\+\+)"},
        {Tok::MinusMinus, R"(--)"},

        {Tok::EqEq, R"(==)"},
        {Tok::BangEq, R"(!=)"},
        {Tok::Lte, R"(<=)"},
        {Tok::Gte, R"(>=)"},
        {Tok::AndAnd, R"(&&)"},
        {Tok::OrOr, R"(\|\|)"},

        {Tok::Plus, R"(\+)"},
        {Tok::Minus, R"(-)"},
        {Tok::Star, R"(\*)"},
        {Tok::Slash, R"(/)"},
        {Tok::Percent, R"(%)"},
        {Tok::Bang, R"(!)"},
        {Tok::Amp, R"(&)"},
        {Tok::Pipe, R"(\|)"},
        {Tok::Eq, R"(=)"},
        {Tok::Lt, R"(<)"},
        {Tok::Gt, R"(>)"},
        {Tok::LParen, R"(\()"},
        {Tok::RParen, R"(\))"},
        {Tok::LBrace, R"(\{)"},
        {Tok::RBrace, R"(\})"},
        {Tok::LBracket, R"(\[)"},
        {Tok::RBracket, R"(\])"},
        {Tok::Comma, R"(,)"},
        {Tok::Dot, R"(\.)"},
        {Tok::Colon, R"(:)"},
    };
    return specs;
  }

  // ----- Keyword table (identifier -> keyword token) -----
  inline const std::unordered_map<std::string, Tok> &keyword_map()
  {
    static const std::unordered_map<std::string, Tok> map = {
        // control flow
        {"if", Tok::KwIf},
        {"else", Tok::KwElse},
        {"sw", Tok::KwSw},
        {"case", Tok::KwCase},
        {"default", Tok::KwDefault},
        {"while", Tok::KwWhile},
        {"do", Tok::KwDo},
        {"for", Tok::KwFor},
        {"in", Tok::KwIn},
        {"try", Tok::KwTry},
        {"catch", Tok::KwCatch},
        {"return", Tok::KwReturn},
        {"break", Tok::KwBreak},
        {"continue", Tok::KwContinue},
        {"raise", Tok::KwRaise},

        // defs / visibility / modules
        {"struct", Tok::KwStruct},
        {"pub", Tok::KwPub},
        {"const", Tok::KwConst},
        {"import", Tok::KwImport},
        {"as", Tok::KwAs},

        // literals / special
        {"true", Tok::KwTrue},
        {"false", Tok::KwFalse},
        {"any", Tok::KwAny},

        // primitive types
        {"i8", Tok::KwI8},
        {"i16", Tok::KwI16},
        {"i32", Tok::KwI32},
        {"i64", Tok::KwI64},
        {"int", Tok::KwInt},
        {"u8", Tok::KwU8},
        {"u16", Tok::KwU16},
        {"u32", Tok::KwU32},
        {"u64", Tok::KwU64},
        {"uint", Tok::KwUint},
        {"bool", Tok::KwBool},
        {"char", Tok::KwChar},
        {"str", Tok::KwStr}};
    return map;
  }

  // ----- Lexing policy helpers -----
  // Treat '\n' as a real token (NL). Skip spaces/tabs/carriage returns.
  // Skip comments by default (or surface them if your pipeline wants them).

} // namespace blam

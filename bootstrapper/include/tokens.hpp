#pragma once

#include <string_view>
#include <array>

#define N_KEYWORDS 23

namespace blam {

enum class Tok : int32_t {
  // structure
  EOF_, Invalid, NL, Ident,

  // primitives
  Int, Float, String, Char,

  // keywords
  KwIf, KwElse, KwElif, 
  KwSw, KwCase, KwDefault, 
  KwWhile, KwDo, KwFor, KwIn, KwReturn, 
  KwBreak, KwContinue, 
  KwTry, KwCatch, KwRaise, 
  KwStruct, KwPub, KwConst, 
  KwImport, KwAs, 
  KwTrue, KwFalse,

  // comments (lexer usually skips, but expose if needed for tools)
  SlComment, // # ... (to end of line)
  MlComment, // #* ... *#

  // operators / punctuators
  Plus, Minus, Multiply, Divide,
  Eq, EqEq, Neq,
  Lt, Lte, Gt, Gte,
  And, Or,
  Inc, Dec,
  Arrow,
  LParen, RParen, LBrace, RBrace, LBracket, RBracket,
  Comma, Dot, Colon
};

// string to token translations
struct TokDirEntry { std::string_view name; Tok value; };
constexpr std::array<TokDirEntry, N_KEYWORDS> tokDir{{
  {"as",       Tok::KwAs},
  {"break",    Tok::KwBreak},
  {"case",     Tok::KwCase},
  {"catch",    Tok::KwCatch},
  {"const",    Tok::KwConst},
  {"continue", Tok::KwContinue},
  {"default",  Tok::KwDefault},
  {"do",       Tok::KwDo},
  {"elif",     Tok::KwElif},
  {"else",     Tok::KwElse},
  {"false",    Tok::KwFalse},
  {"for",      Tok::KwFor},
  {"if",       Tok::KwIf},
  {"import",   Tok::KwImport},
  {"in",       Tok::KwIn},
  {"pub",      Tok::KwPub},
  {"raise",    Tok::KwRaise},
  {"return",   Tok::KwReturn},
  {"struct",   Tok::KwStruct},
  {"sw",       Tok::KwSw},
  {"true",     Tok::KwTrue},
  {"try",      Tok::KwTry},
  {"while",    Tok::KwWhile},
}};


inline const Tok string_to_token(std::string_view s) {
  size_t low = 0;
  size_t high = tokDir.size();

  while (low < high) {
    size_t mid = low + (high - low) / 2;

    if (tokDir[mid].name < s) {
      low = mid + 1;
    } else {
      high = mid;
    }
  }

  if (low < N_KEYWORDS && tokDir[low].name == s) {
    return tokDir[low].value;
  }

  return Tok::Invalid;
}

// token representation
struct Pos { size_t index{0}, line{1}, col{1}; };
struct Range { Pos start{}, end{}; };

struct Token {
  Tok kind{};
  std::string_view value;
  Range range{};
};

} // namespace Blam
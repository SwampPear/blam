#include <string_view>
#include <unordered_map>
#include "tokens.hpp"

namespace blam {

using namespace std::literals;

const std::unordered_map<std::string_view, Tok>& keyword_map() {
  static const std::unordered_map<std::string_view, Tok> m = {
    {"if"sv,       Tok::KwIf},
    {"else"sv,     Tok::KwElse},
    {"sw"sv,   Tok::KwSw},
    {"case"sv,     Tok::KwCase},
    {"default"sv,  Tok::KwDefault},
    {"while"sv,    Tok::KwWhile},
    {"do"sv,       Tok::KwDo},
    {"for"sv,      Tok::KwFor},
    {"in"sv,       Tok::KwIn},
    {"try"sv,      Tok::KwTry},
    {"catch"sv,    Tok::KwCatch},
    {"return"sv,   Tok::KwReturn},
    {"break"sv,    Tok::KwBreak},
    {"continue"sv, Tok::KwContinue},
    {"raise"sv,    Tok::KwRaise},
    {"struct"sv,   Tok::KwStruct},
    {"pub"sv,      Tok::KwPub},
    {"const"sv,    Tok::KwConst},
    {"import"sv,   Tok::KwImport},
    {"as"sv,       Tok::KwAs},
    {"true"sv,     Tok::KwTrue},
    {"false"sv,    Tok::KwFalse}
  };

  return m;
}

} // namespace blam

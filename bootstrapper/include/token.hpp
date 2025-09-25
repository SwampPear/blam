#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <cstdint>

// token types (ordered in parsing hierarchy)
enum class Type : int32_t
{
  // parser helpers
  SKIP = -2, // skippable tokens
  END = 0,   // end of file marker
  RAW = 1,   // raw src

  // comments
  MLINE_COMMENT = 2, // #* *#
  SLINE_COMMENT = 3, // # \n

  // strings
  STRING = 4, // "example"

  // reserved keywords
  AND = 5,
  OR = 6,
  DEF = 7,
  RET = 8,
  IF = 9,
  EL = 10,
  ELIF = 11,
  WHILE = 12,
  FOR = 13,
  IN = 14,
  BREAK = 15,
  CONTINUE = 16,
  TRUE = 17,
  FALSE = 18,
  NIL = 19,
  PUB = 20,
  CONST = 21,
  LET = 22,

  // any text
  IDENT = 23,

  // numbers
  DECIMAL = 24,
  NUMBER = 25,

  // space
  NLINE = 26,
  WHITESPACE = 27,

  // delimiters
  SMBRACKET_L = 28, // (
  SMBRACKET_R = 29, // )
  SQBRACKET_L = 30, // [
  SQBRACKET_R = 31, // ]
  CUBRACKET_L = 32, // {
  CUBRACKET_R = 33, // }

  // operators
  ARROW = 34, // ->
  BAND = 35,  // &
  BOR = 36,   // |
  BXOR = 37,  // ^
  BLS = 38,   // <<
  BRS = 39,   // >>
  DOT = 40,   // .
  COMMA = 41, // ,
  COLON = 42, // :
  AT = 43,    // @

  // comparison
  EQEQ = 44, // ==
  NEQ = 45,  // !=
  LT = 46,   // <
  GT = 47,   // >
  LTE = 48,  // <=
  GTE = 49,  // >=

  // arithmetic
  PLUSEQ = 50, // +=
  MINEQ = 51,  // -=
  DIVEQ = 52,  // /=
  MULTEQ = 53, // *=
  EQ = 54,     // =
  PLUS = 55,   // +
  MIN = 56,    // -
  DIV = 57,    // /
  EXP = 58,    // **
  MULT = 59    // *
};

inline const char *type_to_string(Type t)
{
  switch (t)
  {
  case Type::SKIP:
    return "SKIP";
  case Type::END:
    return "END";
  case Type::RAW:
    return "RAW";
  case Type::MLINE_COMMENT:
    return "MLINE_COMMENT";
  case Type::SLINE_COMMENT:
    return "SLINE_COMMENT";
  case Type::STRING:
    return "STRING";
  case Type::AND:
    return "AND";
  case Type::OR:
    return "OR";
  case Type::DEF:
    return "DEF";
  case Type::RET:
    return "RET";
  case Type::IF:
    return "IF";
  case Type::EL:
    return "EL";
  case Type::ELIF:
    return "ELIF";
  case Type::WHILE:
    return "WHILE";
  case Type::FOR:
    return "FOR";
  case Type::IN:
    return "IN";
  case Type::BREAK:
    return "BREAK";
  case Type::CONTINUE:
    return "CONTINUE";
  case Type::TRUE:
    return "TRUE";
  case Type::FALSE:
    return "FALSE";
  case Type::NIL:
    return "NIL";
  case Type::PUB:
    return "PUB";
  case Type::CONST:
    return "CONST";
  case Type::LET:
    return "LET";
  case Type::IDENT:
    return "IDENT";
  case Type::DECIMAL:
    return "DECIMAL";
  case Type::NUMBER:
    return "NUMBER";
  case Type::NLINE:
    return "NLINE";
  case Type::WHITESPACE:
    return "WHITESPACE";
  case Type::SMBRACKET_L:
    return "SMBRACKET_L";
  case Type::SMBRACKET_R:
    return "SMBRACKET_R";
  case Type::SQBRACKET_L:
    return "SQBRACKET_L";
  case Type::SQBRACKET_R:
    return "SQBRACKET_R";
  case Type::CUBRACKET_L:
    return "CUBRACKET_L";
  case Type::CUBRACKET_R:
    return "CUBRACKET_R";
  case Type::ARROW:
    return "ARROW";
  case Type::BAND:
    return "BAND";
  case Type::BOR:
    return "BOR";
  case Type::BXOR:
    return "BXOR";
  case Type::BLS:
    return "BLS";
  case Type::BRS:
    return "BRS";
  case Type::DOT:
    return "DOT";
  case Type::COMMA:
    return "COMMA";
  case Type::COLON:
    return "COLON";
  case Type::AT:
    return "AT";
  case Type::EQEQ:
    return "EQEQ";
  case Type::NEQ:
    return "NEQ";
  case Type::LT:
    return "LT";
  case Type::GT:
    return "GT";
  case Type::LTE:
    return "LTE";
  case Type::GTE:
    return "GTE";
  case Type::PLUSEQ:
    return "PLUSEQ";
  case Type::MINEQ:
    return "MINEQ";
  case Type::DIVEQ:
    return "DIVEQ";
  case Type::MULTEQ:
    return "MULTEQ";
  case Type::EQ:
    return "EQ";
  case Type::PLUS:
    return "PLUS";
  case Type::MIN:
    return "MIN";
  case Type::DIV:
    return "DIV";
  case Type::EXP:
    return "EXP";
  case Type::MULT:
    return "MULT";
  default:
    return "UNKNOWN";
  }
}

inline const char *type_to_regex(Type t)
{
  switch (t)
  {
  case Type::MLINE_COMMENT:
    return R"(#\*[\s\S]*?\*#)";
  case Type::SLINE_COMMENT:
    return R"(#([^\n]*)(\n|$))";
  case Type::STRING:
    return R"("(\\.|[^"\\])*")";

  case Type::AND:
    return R"(\band\b)";
  case Type::OR:
    return R"(\bor\b)";
  case Type::DEF:
    return R"(\bdef\b)";
  case Type::RET:
    return R"(\bret\b)";
  case Type::IF:
    return R"(\bif\b)";
  case Type::EL:
    return R"(\bel\b)";
  case Type::ELIF:
    return R"(\belif\b)";
  case Type::WHILE:
    return R"(\bwhile\b)";
  case Type::FOR:
    return R"(\bfor\b)";
  case Type::IN:
    return R"(\bin\b)";
  case Type::BREAK:
    return R"(\bbreak\b)";
  case Type::CONTINUE:
    return R"(\bcontinue\b)";
  case Type::TRUE:
    return R"(\btrue\b)";
  case Type::FALSE:
    return R"(\bfalse\b)";
  case Type::NIL:
    return R"(\bnil\b)";
  case Type::PUB:
    return R"(\bpub\b)";
  case Type::CONST:
    return R"(\bconst\b)";
  case Type::LET:
    return R"(\blet\b)";

  case Type::IDENT:
    return R"(\b[a-zA-Z_][a-zA-Z0-9_]*\b)";

  case Type::DECIMAL:
    return R"((?:\d+\.\d*|\.\d+))";
  case Type::NUMBER:
    return R"(\d+)";

  case Type::NLINE:
    return R"(\n)";
  case Type::WHITESPACE:
    return R"(\s+)";

  case Type::SMBRACKET_L:
    return R"(\()";
  case Type::SMBRACKET_R:
    return R"(\))";
  case Type::SQBRACKET_L:
    return R"(\[)";
  case Type::SQBRACKET_R:
    return R"(\])";
  case Type::CUBRACKET_L:
    return R"(\{)";
  case Type::CUBRACKET_R:
    return R"(\})";

  case Type::ARROW:
    return R"(->)";
  case Type::BAND:
    return R"(&)";
  case Type::BOR:
    return R"(\|)";
  case Type::BXOR:
    return R"(\^)";
  case Type::BLS:
    return R"(<<)";
  case Type::BRS:
    return R"(>>)";
  case Type::DOT:
    return R"(\.)";
  case Type::COMMA:
    return R"(,)";
  case Type::COLON:
    return R"(:)";
  case Type::AT:
    return R"(@)";

  case Type::EQEQ:
    return R"(==)";
  case Type::NEQ:
    return R"(!=)";
  case Type::LTE:
    return R"(<=)";
  case Type::GTE:
    return R"(>=)";
  case Type::LT:
    return R"(<)";
  case Type::GT:
    return R"(>)";

  case Type::PLUSEQ:
    return R"(\+=)";
  case Type::MINEQ:
    return R"(-=)";
  case Type::DIVEQ:
    return R"(/=)";
  case Type::MULTEQ:
    return R"(\*=)";
  case Type::EQ:
    return R"(=)";
  case Type::PLUS:
    return R"(\+)";
  case Type::MIN:
    return R"(-)";
  case Type::DIV:
    return R"(/)";
  case Type::EXP:
    return R"(\*\*)";
  case Type::MULT:
    return R"(\*)";

  default:
    return "";
  }
}

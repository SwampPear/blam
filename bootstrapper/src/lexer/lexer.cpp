#include <cctype>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <optional>
#include "lexer.hpp"

namespace blam {

// Tokenizes some source.
std::vector<Token> Lexer::tokenize() {
  std::vector<Token> out;

  while (true) {
    Token t = next();
    out.emplace_back(std::move(t));

    if (out.back().kind == Tok::EOF_) // stop after EOF
      break;
  }

  return out;
}

// Returns const ptr to next token.
const Token &Lexer::peek() {
  if (!has_peek) {
    look = next_impl();
    has_peek = true;
  }

  return look;
}

// Returns next token.
Token Lexer::next() {
  if (has_peek) {
    has_peek = false;
    return look;
  }

  return next_impl();
}

// Updates current position and advances index.
void Lexer::bump() {
  if (at_end()) return;

  if (ch() == '\n') {
    pos.line++;
    pos.col = 1;
  } else {
    pos.col++;
  }

  i++;
  pos.index = i;
}

// Bumps n steps.
void Lexer::bump_n(size_t n) {
  while (n--) bump();
}

// Handles positioning with newlines.
Token Lexer::handle_newline() {
  Pos s = pos;

  if (ch() == '\r') bump(); // process carriage return
  if (ch() == '\n') bump();

  return Token{Tok::NL, "\n", Range{s, pos}};
}

// Matches a string
bool Lexer::match_str(std::string_view s) {
  const size_t n = s.size();

  if (i + n > src.size()) return false;

  if (src.substr(i, n).compare(s) == 0) {
    bump_n(n);
    return true;
  }

  return false;
}

// Skips horiztonal whitespace if any.
void Lexer::skip_horizontal_ws() {
  while (!at_end() && is_space_non_nl(ch())) {
    if (ch() == '\r' && ch(1) != '\n') {
      bump();
      continue;
    }

    bump();
  }
}

// Skips comments if any.
bool Lexer::skip_comment_if_any(std::optional<Token> &surfaced) {
  if (ch() == '#') {
    Pos s = pos;

    if (ch(1) == '*') { // multi-line
      bump_n(2); // consume '#*'

      size_t start = i;
      // consume comment
      while (!at_end() && !(ch() == '*' && ch(1) == '#')) {
        bump();
      }
      size_t end = i;

      if (at_end())
        throw LexError("Unterminated multi-line comment", Range{s, pos});

      bump_n(2); // consume *#

      if (comments) {
        std::string_view lex = src.substr(start, end - start);
        surfaced = Token{Tok::MlComment, lex, Range{s, pos}};
      }

      return true;
    } else {
      bump(); // consume '#'

      // consume comment
      size_t start = i;
      while (!at_end() && ch() != '\n')
        bump();
      size_t end = i;

      if (comments) {
        std::string_view lex = src.substr(start, end - start);
        surfaced = Token{Tok::SlComment, lex, Range{s, pos}};
      }

      return true;
    }
  }

  return false;
}

// Scans alphanumeric words.
Token Lexer::scan_identifier_or_keyword() {
  Pos s = pos;
  size_t start = i;

  bump(); // consume first char

  // consume word
  while (!at_end() && is_ident_cont(ch())) bump(); 

  std::string_view lex = src.substr(start, i - start);

  // get keyword type
  Tok tok = string_to_token(lex);
  if (tok != Tok::Invalid) {
    return Token{tok, lex, Range{s, pos}};
  }

  return Token{Tok::Ident, lex, Range{s, pos}};
}

Token Lexer::scan_number_or_float() {
  Pos s = pos;
  size_t start = i;

  Tok tok = Tok::Int;

  while (is_digit(ch()) || ch() == '.') {
    bump();

    if (ch() == '.') tok = Tok::Float;
  }

  std::string_view lex = src.substr(start, i - start);

  return Token{tok, lex, Range{s, pos}};
}

/*
  Token Lexer::scan_string_like(char quote, Tok kind)
  {
    Pos s = pos_;
    size_t start = i_;
    bump(); // opening quote
    bool escaped = false;
    while (!at_end())
    {
      char c = ch();
      bump();
      if (escaped)
      {
        escaped = false;
        continue;
      }
      if (c == '\\')
      {
        escaped = true;
        continue;
      }
      if (c == quote)
      {
        std::string_view lex = src_.substr(start, i_ - start);
        return Token{kind, lex, Range{s, pos}};
      }
      if (c == '\n')
      {
        throw LexError("Unterminated string/char literal before newline", Range{s, pos_});
      }
    }
    throw LexError("Unterminated string/char literal", Range{s, pos_});
  }
  */

// Scans for operators and punctuators.
Token Lexer::scan_operator_or_punct() {
  Pos s = pos;

  // 2-char operators
  if (ch() == '=' && ch(1) == '=')
    return Token{Tok::EqEq, "==", Range{s, pos}};
  if (ch() == '<' && ch(1) == '=')
    return Token{Tok::Lte, "<=", Range{s, pos}};
  if (ch() == '>' && ch(1) == '=')
    return Token{Tok::Gte, ">=", Range{s, pos}};
  
  // 1-char punctuators/operators
  switch (ch()) {
  case '+':
    bump();
    return Token{Tok::Plus, "+", Range{s, pos}};
  case '-':
    bump();
    return Token{Tok::Minus, "-", Range{s, pos}};
  case '*':
    bump();
    return Token{Tok::Multiply, "*", Range{s, pos}};
  case '/':
    bump();
    return Token{Tok::Divide, "/", Range{s, pos}};
  case '=':
    bump();
    return Token{Tok::Eq, "=", Range{s, pos}};
  case '<':
    bump();
    return Token{Tok::Lt, "<", Range{s, pos}};
  case '>':
    bump();
    return Token{Tok::Gt, ">", Range{s, pos}};
  case '(':
    bump();
    return Token{Tok::LParen, "(", Range{s, pos}};
  case ')':
    bump();
    return Token{Tok::RParen, ")", Range{s, pos}};
  case '{':
    bump();
    return Token{Tok::LBrace, "{", Range{s, pos}};
  case '}':
    bump();
    return Token{Tok::RBrace, "}", Range{s, pos}};
  case '[':
    bump();
    return Token{Tok::LBracket, "[", Range{s, pos}};
  case ']':
    bump();
    return Token{Tok::RBracket, "]", Range{s, pos}};
  case ',':
    bump();
    return Token{Tok::Comma, ",", Range{s, pos}};
  case '.':
    bump();
    return Token{Tok::Dot, ".", Range{s, pos}};
  case ':':
    bump();
    return Token{Tok::Colon, ":", Range{s, pos}};
  default:
    break;
  }

  return Token{Tok::Invalid, "", Range{s, s}}; // treat as invalid
}

Token Lexer::next_impl() {
  // skip spaces, tabs, cr
  skip_horizontal_ws();

  if (at_end()) {
    Pos s = pos;
    return Token{Tok::EOF_, "", Range{s, s}};
  }

  // newline again after skipping spaces
  if (ch() == '\n' || (ch() == '\r' && ch(1) == '\n')) {
    return handle_newline();
  }

  // comments (optionally surfaced)
  std::optional<Token> surfaced;
  if (skip_comment_if_any(surfaced)) {
    if (comments && surfaced.has_value())
      return *surfaced;

    return next_impl();
  }

  // identifiers / keywords
  if (is_ident_start(ch())) {
    return scan_identifier_or_keyword();
  }

  if (is_digit(ch()) || ch() == '.') {
    return scan_number_or_float();
  }

  // Strings / chars
  /*
  if (ch() == '"')
    return scan_string_like('"', Tok::String);
  if (ch() == '\'')
    return scan_string_like('\'', Tok::Char);
    */

  // operators / punctuators
  Token op = scan_operator_or_punct();
  if (op.kind != Tok::Invalid) return op;

  // Unknown character
  Pos s = pos;
  char bad = ch();
  bump();

  throw LexError(std::string("Unexpected character: '") + bad + "'", Range{s, pos});
}

} // namespace blam

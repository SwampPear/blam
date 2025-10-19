#include <cctype>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <optional>
#include "lexer.hpp"

namespace blam {
  std::vector<Token> Lexer::tokenize() {
    std::vector<Token> out; // token stream
    Token t{};              // current token

    do {
      t = next();       // process, iterate
      out.push_back(t); // push to token stream
    } while (t.kind != Tok::EOF_);
    
    return out;
  }

  const Token &Lexer::peek() {
    // lookahead not cached
    if (!has_peek_) {
      look_ = next_impl();
      has_peek_ = true;
    }

    return look_;
  }

  Token Lexer::next()
  {
    // lookahead cached
    if (has_peek_) {
      has_peek_ = false;
      return look_;
    }

    return next_impl();
  }

  bool Lexer::at_end() const { return i_ >= src_.size(); }
  char Lexer::ch(size_t k) const { return (i_ + k < src_.size()) ? src_[i_ + k] : '\0'; }

  void Lexer::bump() {
    if (at_end()) return;

    if (ch() == '\n') {
      pos_.line++;
      pos_.col = 1;
    } else {
      pos_.col++;
    }

    i_++;
    pos_.index = i_;
  }

  void Lexer::bump_n(size_t n) {
    while (n--) bump();
  }

  Token Lexer::make(Tok k, std::string_view lex, Pos start, Pos end) const {
    return Token{k, std::string(lex), Range{start, end}};
  }

  Token Lexer::handle_newline() {
    Pos s = pos_;

    if (ch() == '\r' && ch(1) == '\n') {
      bump();
      bump();
    } else {
      bump();
    }

    return make(Tok::NL, "\n", s, pos_);
  }

  bool Lexer::match_str(const char *s) {
    size_t n = std::char_traits<char>::length(s);

    if (src_.substr(i_, n) == s) {
      bump_n(n);
      return true;
    }

    return false;
  }

  void Lexer::skip_horizontal_ws() {
    while (!at_end() && is_space_non_nl(ch())) {
      if (ch() == '\r' && ch(1) != '\n') {
        bump();
        continue;
      }

      bump();
    }
  }

  bool Lexer::skip_comment_if_any(std::optional<Token> &surfaced) {
    if (ch() == '#') {
      Pos s = pos_;
      if (ch(1) == '*') {
        // multi-line
        bump_n(2);
        while (!at_end() && !(ch() == '*' && ch(1) == '#')) {
          bump();
        }

        if (at_end())
          throw LexError("Unterminated multi-line comment", Range{s, pos_});

        bump_n(2); // consume *#

        if (keep_comments_) {
          surfaced = make(Tok::MlComment, "", s, pos_);
        }

        return true;
      } else
      {
        // consume '#
        bump(); // consume '#'

        // consume comment
        while (!at_end() && ch() != '\n')
          bump();

        if (keep_comments_) {
          surfaced = make(Tok::SlComment, "", s, pos_);
        }

        return true;
      }
    }

    return false;
  }

  Token Lexer::scan_identifier_or_keyword() {
    Pos s = pos_;
    size_t start = i_;

    bump(); // first char

    while (!at_end() && is_ident_cont(ch()))
      bump();

    std::string_view lex = src_.substr(start, i_ - start);

    auto it = keyword_map().find(std::string(lex));
    if (it != keyword_map().end()) return make(it->second, lex, s, pos_);

    return make(Tok::Identifier, lex, s, pos_);
  }

  std::optional<Token> Lexer::scan_number_or_float()
  {
    Pos s = pos_;
    size_t start = i_;
    bool seen_dot = false;

    auto is_num = [](char c)
    { return std::isdigit(static_cast<unsigned char>(c)); };

    // patterns allowed: 123, 123., .123, 123.456
    if (ch() == '.')
    {
      // .123
      if (!is_num(ch(1)))
      {
        return std::nullopt; // just a dot; let caller handle as operator
      }
      seen_dot = true;
      bump(); // '.'
      while (is_num(ch()))
        bump();
    }
    else
    {
      // digits
      if (!is_num(ch()))
        return std::nullopt;
      while (is_num(ch()))
        bump();
      if (ch() == '.')
      {
        seen_dot = true;
        bump();
        while (is_num(ch()))
          bump();
      }
    }

    std::string_view lex = src_.substr(start, i_ - start);
    return seen_dot ? std::optional<Token>(make(Tok::Float, lex, s, pos_))
                    : std::optional<Token>(make(Tok::Int, lex, s, pos_));
  }

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
        return make(kind, lex, s, pos_);
      }
      if (c == '\n')
      {
        throw LexError("Unterminated string/char literal before newline", Range{s, pos_});
      }
    }
    throw LexError("Unterminated string/char literal", Range{s, pos_});
  }

  // Multi-char ops first
  Token Lexer::scan_operator_or_punct()
  {
    Pos s = pos_;

    auto try_make = [&](Tok k, const char *lit) -> std::optional<Token>
    {
      size_t n = std::char_traits<char>::length(lit);
      if (src_.substr(i_, n) == lit)
      {
        bump_n(n);
        return make(k, std::string_view(lit, n), s, pos_);
      }
      return std::nullopt;
    };

    // 2-char operators
    if (auto t = try_make(Tok::Arrow, "->"))
      return *t;
    if (auto t = try_make(Tok::EqEq, "=="))
      return *t;
    if (auto t = try_make(Tok::BangEq, "!="))
      return *t;
    if (auto t = try_make(Tok::Lte, "<="))
      return *t;
    if (auto t = try_make(Tok::Gte, ">="))
      return *t;
    if (auto t = try_make(Tok::AndAnd, "&&"))
      return *t;
    if (auto t = try_make(Tok::OrOr, "||"))
      return *t;
    if (auto t = try_make(Tok::PlusPlus, "++"))
      return *t;
    if (auto t = try_make(Tok::MinusMinus, "--"))
      return *t;

    // 1-char punctuators/operators
    switch (ch())
    {
    case '+':
      bump();
      return make(Tok::Plus, "+", s, pos_);
    case '-':
      bump();
      return make(Tok::Minus, "-", s, pos_);
    case '*':
      bump();
      return make(Tok::Star, "*", s, pos_);
    case '/':
      bump();
      return make(Tok::Slash, "/", s, pos_);
    case '%':
      bump();
      return make(Tok::Percent, "%", s, pos_);
    case '!':
      bump();
      return make(Tok::Bang, "!", s, pos_);
    case '&':
      bump();
      return make(Tok::Amp, "&", s, pos_);
    case '|':
      bump();
      return make(Tok::Pipe, "|", s, pos_);
    case '=':
      bump();
      return make(Tok::Eq, "=", s, pos_);
    case '<':
      bump();
      return make(Tok::Lt, "<", s, pos_);
    case '>':
      bump();
      return make(Tok::Gt, ">", s, pos_);
    case '(':
      bump();
      return make(Tok::LParen, "(", s, pos_);
    case ')':
      bump();
      return make(Tok::RParen, ")", s, pos_);
    case '{':
      bump();
      return make(Tok::LBrace, "{", s, pos_);
    case '}':
      bump();
      return make(Tok::RBrace, "}", s, pos_);
    case '[':
      bump();
      return make(Tok::LBracket, "[", s, pos_);
    case ']':
      bump();
      return make(Tok::RBracket, "]", s, pos_);
    case ',':
      bump();
      return make(Tok::Comma, ",", s, pos_);
    case '.':
      bump();
      return make(Tok::Dot, ".", s, pos_);
    case ':':
      bump();
      return make(Tok::Colon, ":", s, pos_);
    default:
      break;
    }
    return make(Tok::EOF_, "", s, s); // caller should treat as error if not actually at end
  }

  Token Lexer::next_impl()
  {
    // End?
    if (at_end())
    {
      Pos s = pos_;
      return make(Tok::EOF_, "", s, s);
    }

    // Newline?
    if (ch() == '\n' || (ch() == '\r' && ch(1) == '\n'))
    {
      return handle_newline();
    }

    // Skip spaces/tabs/CR (non-NL)
    skip_horizontal_ws();
    if (at_end())
    {
      Pos s = pos_;
      return make(Tok::EOF_, "", s, s);
    }

    // Newline again after skipping spaces?
    if (ch() == '\n' || (ch() == '\r' && ch(1) == '\n'))
    {
      return handle_newline();
    }

    // Comments (optionally surfaced)
    std::optional<Token> surfaced;
    if (skip_comment_if_any(surfaced))
    {
      if (keep_comments_ && surfaced.has_value())
        return *surfaced;
      if (!at_end() && (ch() == '\n' || (ch() == '\r' && ch(1) == '\n')))
      {
        return handle_newline();
      }
      return next_impl();
    }

    // Identifiers / keywords
    if (is_ident_start(ch()))
    {
      return scan_identifier_or_keyword();
    }

    // Numbers (int/float). Special-case leading '.' -> float like .5; otherwise '.' is Dot
    if (auto num = scan_number_or_float())
    {
      return *num;
    }

    // Strings / chars
    if (ch() == '"')
      return scan_string_like('"', Tok::String);
    if (ch() == '\'')
      return scan_string_like('\'', Tok::Char);

    // Operators / punctuators
    {
      Token op = scan_operator_or_punct();
      if (op.kind != Tok::EOF_)
        return op;
    }

    // Unknown character
    Pos s = pos_;
    char bad = ch();
    bump();
    throw LexError(std::string("Unexpected character: '") + bad + "'", Range{s, pos_});
  }
} // namespace blam

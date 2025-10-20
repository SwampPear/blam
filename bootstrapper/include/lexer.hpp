#pragma once

#include <cctype>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <optional>
#include "tokens.hpp"

namespace blam {

struct LexError : std::runtime_error {
  Range range{};
  explicit LexError(const std::string &msg, Range r) : std::runtime_error(msg), range(r) {}
};

  class Lexer {
  public:
    explicit Lexer(std::string_view src, bool comments = false) : src(src), comments(comments) {}

    std::vector<Token> tokenize();
    const Token &peek();
    Token next();

  private:
    std::string_view src; // original source buffer
    bool comments{false}; // surface comments as tokens or discard them
    size_t i{0};          // current index into the source
    Pos pos{};            // current logical source position (line/columne)
    bool has_peek{false}; // whether the lookahead cache is populated
    Token look{};         // cached lookahead token

    static bool is_ident_start(char c) { return std::isalpha(static_cast<unsigned char>(c)) || c == '_'; }
    static bool is_ident_cont(char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; }
    static bool is_digit(char c) { return std::isdigit(static_cast<unsigned char>(c)); }
    static bool is_space_non_nl(char c) { return c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r'; }
    static bool is_operator_start(char c) {
      switch(c) {
      case '+': 
        return true;
      case '-':
        return true;
      case '*':
        return true;
      case '/':
        return true;
      case '=':
        return true;
      case '<':
        return true;
      case '>':
        return true;
      case '(':
        return true;
      case ')':
        return true;
      case '{':
        return true;
      case '}':
        return true;
      case '[':
        return true;
      case ']':
        return true;
      case ',':
        return true;
      case '.':
        return true;
      case ':':
        return true;
      default:
        break;
      }

      return false;
    }

    bool at_end() const { return i >= src.size(); }
    char ch(size_t k = 0) const { return (i + k < src.size()) ? src[i + k] : '\0'; }

    void bump();
    void bump_n(size_t n);

    Token handle_newline();
    bool match_str(std::string_view s);
    void skip_horizontal_ws();
    bool skip_comment_if_any(std::optional<Token> &surfaced);
    Token scan_identifier_or_keyword();
    Token scan_number_or_float();
    //Token scan_string_like(char quote, Tok kind);
    Token scan_operator_or_punct();
    Token next_impl();
  };
} // namespace blam
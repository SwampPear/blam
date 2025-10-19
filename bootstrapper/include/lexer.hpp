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
    explicit Lexer(std::string_view src, bool keep_comments = false)
        : src_(src), keep_comments_(keep_comments) {}

    std::vector<Token> tokenize();
    const Token &peek();
    Token next();

  private:
    std::string_view src_;      // original source buffer (not null-terminated, non-owning view)
    bool keep_comments_{false}; // surface comments as tokens or discard them
    size_t i_{0};               // current byte index into the source
    Pos pos_{};                 // current logical source position (line/column)
    bool has_peek_{false};      // whether the lookahead cache is populated
    Token look_{};              // cached lookahead token when has_peek_ is true

    bool at_end() const;

    char ch(size_t k = 0) const;

    void bump();
    void bump_n(size_t n);

    static bool is_ident_start(char c) { return std::isalpha(static_cast<unsigned char>(c)) || c == '_'; }
    static bool is_ident_cont(char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; }
    static bool is_digit(char c) { return std::isdigit(static_cast<unsigned char>(c)); }
    static bool is_space_non_nl(char c) { return c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r'; }

    Token make(Tok k, std::string_view lex, Pos start, Pos end) const;
    Token handle_newline();
    bool match_str(const char *s);
    void skip_horizontal_ws();
    bool skip_comment_if_any(std::optional<Token> &surfaced);
    Token scan_identifier_or_keyword();
    std::optional<Token> scan_number_or_float();
    Token scan_string_like(char quote, Tok kind);
    Token scan_operator_or_punct();
    Token next_impl();
  };
} // namespace blam
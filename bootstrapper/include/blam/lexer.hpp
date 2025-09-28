#pragma once

#include <cctype>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <optional>
#include "tokens.hpp"

namespace blam
{
  // Lexer error.
  struct LexError : std::runtime_error
  {
    Range range{};
    explicit LexError(const std::string &msg, Range r) : std::runtime_error(msg), range(r) {}
  };

  class Lexer
  {
  public:
    explicit Lexer(std::string_view src, bool keep_comments = false)
        : src_(src), keep_comments_(keep_comments) {}

    // Get all tokens (including NL), ending with EOF_.
    std::vector<Token> tokenize();

    // Peek next token without consuming (cheap: caches one lookahead).
    const Token &peek();

    // Consume and return next token
    Token next();

  private:
    std::string_view src_;
    bool keep_comments_{false};
    size_t i_{0};
    Pos pos_{};
    bool has_peek_{false};
    Token look_{};

    bool at_end() const;
    char ch(size_t k = 0) const;

    void bump();
    void bump_n(size_t n)
    {
      while (n--)
        bump();
    }

    static bool is_ident_start(char c) { return std::isalpha(static_cast<unsigned char>(c)) || c == '_'; }
    static bool is_ident_cont(char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; }
    static bool is_digit(char c) { return std::isdigit(static_cast<unsigned char>(c)); }
    static bool is_space_non_nl(char c) { return c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r'; }

    Token make(Tok k, std::string_view lex, Pos start, Pos end) const;

    // Handle CRLF -> single NL token, CR-alone skipped
    Token handle_newline();

    bool match_str(const char *s);

    // Skip spaces/tabs/CR (but not NL)
    void skip_horizontal_ws();

    // Comments: #... EOL, #* ... *#
    // Returns true if a comment was consumed.
    bool skip_comment_if_any(std::optional<Token> &surfaced);

    Token scan_identifier_or_keyword();

    // returns a token if a number/float was scanned; otherwise std::nullopt
    std::optional<Token> scan_number_or_float();

    Token scan_string_like(char quote, Tok kind);

    // Multi-char ops first
    Token scan_operator_or_punct();

    Token next_impl();
  };
} // namespace blam

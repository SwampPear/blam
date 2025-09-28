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
  // Exception type for lexical errors with source range context.
  struct LexError : std::runtime_error
  {
    Range range{};
    explicit LexError(const std::string &msg, Range r) : std::runtime_error(msg), range(r) {}
  };

  // Hand-rolled lexer for Blam that converts source text into a token stream.
  class Lexer
  {
  public:
    explicit Lexer(std::string_view src, bool keep_comments = false)
        : src_(src), keep_comments_(keep_comments) {}

    // Tokenize the entire input (including NL) and append a final EOF_ token.
    std::vector<Token> tokenize();

    // Peek the next token without consuming it (uses a single-token lookahead cache).
    const Token &peek();

    // Consume and return the next token from the stream.
    Token next();

  private:
    std::string_view src_;      // original source buffer (not null-terminated, non-owning view)
    bool keep_comments_{false}; // surface comments as tokens or discard them
    size_t i_{0};               // current byte index into the source
    Pos pos_{};                 // current logical source position (line/column)
    bool has_peek_{false};      // whether the lookahead cache is populated
    Token look_{};              // cached lookahead token when has_peek_ is true

    // Return true if the cursor is at or beyond the end of the buffer.
    bool at_end() const;

    // Return the current character(or k - ahead) without advancing; '\0' at EOF.
    char ch(size_t k = 0) const;

    // Advance one character, updating index and position.
    void bump();

    // Advance n characters, clamping at EOF.
    void bump_n(size_t n);

    // True if c can start an identifier.
    static bool is_ident_start(char c) { return std::isalpha(static_cast<unsigned char>(c)) || c == '_'; }

    // True if c can continue an identifier.
    static bool is_ident_cont(char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; }

    // True if c is an ASCII digit.
    static bool is_digit(char c) { return std::isdigit(static_cast<unsigned char>(c)); }

    // True if c is horizontal ASCII whitespace (excludes '\n').
    static bool is_space_non_nl(char c) { return c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r'; }

    // Construct a Token with kind, lexeme, and half-open [start,end) range.
    Token make(Tok k, std::string_view lex, Pos start, Pos end) const;

    // Normalize newlines: consume CRLF as a single NL token; skip lone CRs.
    Token handle_newline();

    // If the upcoming characters match s, consume them and return true.
    bool match_str(const char *s);

    // Skip horizontal whitespace (spaces/tabs/CR) but not newlines.
    void skip_horizontal_ws();

    // Consume a line or block comment if present; optionally surface as a token. Return true if comment was consumed.
    bool skip_comment_if_any(std::optional<Token> &surfaced);

    // Scan an identifier or keyword starting at the current position.
    Token scan_identifier_or_keyword();

    // Scan an integer or floating literal; return std::nullopt if not a number.
    std::optional<Token> scan_number_or_float();

    // Scan a quoted string or char literal given its opening quote and token kind.
    Token scan_string_like(char quote, Tok kind);

    // Scan an operator or punctuation token, preferring multi-character forms.
    Token scan_operator_or_punct();

    // Core tokenizer: produce the next token without using the public cache.
    Token next_impl();
  };
} // namespace blam
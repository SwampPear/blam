#pragma once
#include "tokens.hpp"
#include <vector>
#include <unordered_map>

namespace blam
{

  class Lexer
  {
  public:
    explicit Lexer(std::string_view src);
    std::vector<Token> tokenize(); // returns tokens ending with EOF

  private:
    // source
    std::string input_;
    size_t i_ = 0;
    Pos pos_{};

    // helpers
    bool eof() const { return i_ >= input_.size(); }
    char peek(size_t n = 0) const { return (i_ + n < input_.size()) ? input_[i_ + n] : '\0'; }
    std::string_view peekN(size_t n) const { return std::string_view(input_).substr(i_, n); }
    char advance();
    void advanceN(size_t n);
    Pos curPos() const { return pos_; }
    Range rangeFrom(Pos start) const { return Range{start, pos_}; }
    [[noreturn]] void error(Pos at, const char *msg) const;

    // skipping
    void skipHorizontalWS();
    bool lineComment();  // # ... \n
    bool blockComment(); // #* ... *# (nested)

    // emit tokens
    void emitNL(std::vector<Token> &out);

    // lexers
    Token lexInt();
    Token lexString();
    Token lexChar();
    Token lexIdentOrKeyword();
    bool lexMultiOp(std::vector<Token> &out);

    // classification
    static bool isDigit(char c) { return c >= '0' && c <= '9'; }
    static bool isIdentStart(char c) { return (c == '_') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
    static bool isIdentCont(char c) { return isIdentStart(c) || isDigit(c); }

    static Tok singleCharKind(char c);

    static Tok kw(std::string_view s, bool *matched);
  };

} // namespace blam

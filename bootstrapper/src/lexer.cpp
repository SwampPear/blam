#include "lexer.hpp"
#include <iostream>

namespace blam
{

  static const std::unordered_map<std::string, Tok> KEYWORDS{
      {"if", Tok::KwIf}, {"else", Tok::KwElse}, {"sw", Tok::KwSw}, {"case", Tok::KwCase}, {"default", Tok::KwDefault}, {"while", Tok::KwWhile}, {"do", Tok::KwDo}, {"for", Tok::KwFor}, {"in", Tok::KwIn}, {"try", Tok::KwTry}, {"catch", Tok::KwCatch}, {"return", Tok::KwReturn}, {"break", Tok::KwBreak}, {"continue", Tok::KwContinue}, {"raise", Tok::KwRaise}, {"struct", Tok::KwStruct}, {"pub", Tok::KwPub}, {"const", Tok::KwConst}, {"import", Tok::KwImport}, {"as", Tok::KwAs}, {"true", Tok::KwTrue}, {"false", Tok::KwFalse}, {"any", Tok::KwAny}, {"i8", Tok::KwI8}, {"i16", Tok::KwI16}, {"i32", Tok::KwI32}, {"i64", Tok::KwI64}, {"int", Tok::KwInt}, {"u8", Tok::KwU8}, {"u16", Tok::KwU16}, {"u32", Tok::KwU32}, {"u64", Tok::KwU64}, {"uint", Tok::KwUint}, {"bool", Tok::KwBool}, {"char", Tok::KwChar}, {"str", Tok::KwStr}};

  Lexer::Lexer(std::string_view src)
  {
    // normalize newlines to LF
    input_.reserve(src.size());
    for (size_t k = 0; k < src.size(); ++k)
    {
      char c = src[k];
      if (c == '\r')
      {
        if (k + 1 < src.size() && src[k + 1] == '\n')
        { /* CRLF -> handled by skipping */
        }
        c = '\n';
      }
      input_.push_back(c);
    }
  }

  char Lexer::advance()
  {
    char c = peek();
    ++i_;
    if (c == '\n')
    {
      pos_.line++;
      pos_.col = 1;
    }
    else
    {
      pos_.col++;
    }
    pos_.index++;
    return c;
  }

  void Lexer::advanceN(size_t n)
  {
    while (n--)
      advance();
  }

  [[noreturn]] void Lexer::error(Pos at, const char *msg) const
  {
    throw std::runtime_error("lex error @ " + std::to_string(at.line) + ":" + std::to_string(at.col) + ": " + msg);
  }

  void Lexer::skipHorizontalWS()
  {
    while (!eof())
    {
      char c = peek();
      if (c == ' ' || c == '\t')
      {
        advance();
        continue;
      }
      break;
    }
  }

  bool Lexer::lineComment()
  {
    if (peek() == '#' && peekN(2) != "#*")
    {
      while (!eof() && peek() != '\n')
        advance();
      return true;
    }
    return false;
  }

  bool Lexer::blockComment()
  {
    if (peek() == '#' && peekN(2) == "#*")
    {
      advance();
      advance(); // '#', '*'
      int depth = 1;
      while (!eof() && depth > 0)
      {
        auto two = peekN(2);
        if (two == "#*")
        {
          advanceN(2);
          depth++;
          continue;
        }
        if (two == "*#")
        {
          advanceN(2);
          depth--;
          continue;
        }
        advance();
      }
      if (depth != 0)
        error(curPos(), "unterminated block comment");
      return true;
    }
    return false;
  }

  void Lexer::emitNL(std::vector<Token> &out)
  {
    while (peek() == '\n')
    {
      Pos s = curPos();
      advance();
      out.push_back(Token{Tok::NL, "\n", rangeFrom(s)});
    }
  }

  Tok Lexer::singleCharKind(char c)
  {
    switch (c)
    {
    case '+':
      return Tok::Plus;
    case '-':
      return Tok::Minus;
    case '*':
      return Tok::Star;
    case '/':
      return Tok::Slash;
    case '%':
      return Tok::Percent;
    case '!':
      return Tok::Bang;
    case '&':
      return Tok::Amp;
    case '|':
      return Tok::Pipe;
    case '=':
      return Tok::Eq;
    case '<':
      return Tok::Lt;
    case '>':
      return Tok::Gt;
    case '(':
      return Tok::LParen;
    case ')':
      return Tok::RParen;
    case '{':
      return Tok::LBrace;
    case '}':
      return Tok::RBrace;
    case '[':
      return Tok::LBracket;
    case ']':
      return Tok::RBracket;
    case ',':
      return Tok::Comma;
    case '.':
      return Tok::Dot;
    case ':':
      return Tok::Colon;
    default:
      return Tok::EOF_; // sentinel meaning "not a single-char token here"
    }
  }

  Token Lexer::lexInt()
  {
    Pos s = curPos();
    std::string buf;
    while (isDigit(peek()))
      buf.push_back(advance());
    return Token{Tok::Int, std::move(buf), rangeFrom(s)};
  }

  Token Lexer::lexString()
  {
    Pos s = curPos();
    std::string buf;
    buf.push_back(advance()); // opening "
    while (!eof())
    {
      char c = advance();
      buf.push_back(c);
      if (c == '\\')
      {
        buf.push_back(advance());
        continue;
      }
      if (c == '"')
        break;
      if (c == '\n')
        error(s, "unterminated string");
    }
    return Token{Tok::String, std::move(buf), rangeFrom(s)};
  }

  Token Lexer::lexChar()
  {
    Pos s = curPos();
    std::string buf;
    buf.push_back(advance()); // '
    char c = advance();
    buf.push_back(c);
    if (c == '\\')
      buf.push_back(advance());
    char endq = advance();
    buf.push_back(endq);
    if (endq != '\'')
      error(s, "unterminated char");
    return Token{Tok::Char, std::move(buf), rangeFrom(s)};
  }

  Tok Lexer::kw(std::string_view s, bool *matched)
  {
    auto it = KEYWORDS.find(std::string(s));
    if (it != KEYWORDS.end())
    {
      if (matched)
        *matched = true;
      return it->second;
    }
    if (matched)
      *matched = false;
    return Tok::Identifier;
  }

  Token Lexer::lexIdentOrKeyword()
  {
    Pos s = curPos();
    std::string buf;
    buf.push_back(advance());
    while (isIdentCont(peek()))
      buf.push_back(advance());
    bool matched = false;
    Tok kind = kw(buf, &matched);
    return Token{matched ? kind : Tok::Identifier, std::move(buf), rangeFrom(s)};
  }

  bool Lexer::lexMultiOp(std::vector<Token> &out)
  {
    Pos s = curPos();
    auto two = std::string(peekN(2));
    if (two == "->")
    {
      advanceN(2);
      out.push_back(Token{Tok::Arrow, "->", rangeFrom(s)});
      return true;
    }
    if (two == "==")
    {
      advanceN(2);
      out.push_back(Token{Tok::EqEq, "==", rangeFrom(s)});
      return true;
    }
    if (two == "!=")
    {
      advanceN(2);
      out.push_back(Token{Tok::BangEq, "!=", rangeFrom(s)});
      return true;
    }
    if (two == "<=")
    {
      advanceN(2);
      out.push_back(Token{Tok::Lte, "<=", rangeFrom(s)});
      return true;
    }
    if (two == ">=")
    {
      advanceN(2);
      out.push_back(Token{Tok::Gte, ">=", rangeFrom(s)});
      return true;
    }
    if (two == "&&")
    {
      advanceN(2);
      out.push_back(Token{Tok::AndAnd, "&&", rangeFrom(s)});
      return true;
    }
    if (two == "||")
    {
      advanceN(2);
      out.push_back(Token{Tok::OrOr, "||", rangeFrom(s)});
      return true;
    }
    if (two == "++")
    {
      advanceN(2);
      out.push_back(Token{Tok::PlusPlus, "++", rangeFrom(s)});
      return true;
    }
    if (two == "--")
    {
      advanceN(2);
      out.push_back(Token{Tok::MinusMinus, "--", rangeFrom(s)});
      return true;
    }
    return false;
  }

  std::vector<Token> Lexer::tokenize()
  {
    std::vector<Token> out;
    while (!eof())
    {
      skipHorizontalWS();

      if (lineComment())
        continue;
      if (blockComment())
        continue;

      if (peek() == '\n')
      {
        emitNL(out);
        continue;
      }
      if (eof())
        break;

      // literals
      if (peek() == '"')
      {
        out.push_back(lexString());
        continue;
      }
      if (peek() == '\'')
      {
        out.push_back(lexChar());
        continue;
      }
      if (isDigit(peek()))
      {
        out.push_back(lexInt());
        continue;
      }

      // identifiers / keywords
      if (isIdentStart(peek()))
      {
        out.push_back(lexIdentOrKeyword());
        continue;
      }

      // operators
      if (lexMultiOp(out))
        continue;

      // single-char tokens
      Pos s = curPos();
      char c = advance();
      Tok k = singleCharKind(c);
      if (k != Tok::EOF_)
      {
        out.push_back(Token{k, std::string(1, c), rangeFrom(s)});
        continue;
      }

      // ignore stray semicolons (grammar is newline-terminated)
      if (c == ';')
        continue;

      error(s, "unexpected character");
    }

    // push EOF
    out.push_back(Token{Tok::EOF_, "", Range{curPos(), curPos()}});
    return out;
  }

} // namespace blam

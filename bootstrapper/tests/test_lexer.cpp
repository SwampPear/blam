// tests/test_lexer.cpp
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <string>
#include <vector>
#include "lexer.hpp"
#include "tokens.hpp"

using namespace blam;

static std::vector<Tok> kinds(const std::vector<Token> &ts)
{
  std::vector<Tok> ks;
  ks.reserve(ts.size());
  for (auto &t : ts)
    ks.push_back(t.kind);
  return ks;
}

static std::vector<std::string> lexemes(const std::vector<Token> &ts)
{
  std::vector<std::string> xs;
  xs.reserve(ts.size());
  for (auto &t : ts)
    xs.push_back(t.lexeme);
  return xs;
}

static void require_seq(const std::vector<Token> &ts, std::initializer_list<Tok> expected)
{
  auto ks = kinds(ts);
  REQUIRE(ks.size() >= expected.size());
  size_t i = 0;
  for (Tok k : expected)
  {
    INFO("index = " << i);
    REQUIRE(ks[i++] == k);
  }
}

TEST_CASE("empty input yields only EOF")
{
  Lexer lx("");
  auto ts = lx.tokenize();
  REQUIRE(ts.size() == 1);
  REQUIRE(ts[0].kind == Tok::EOF_);
}

TEST_CASE("newlines are produced, CRLF normalized")
{
  Lexer lx("a\nb\r\nc");
  auto ts = lx.tokenize();
  // id, NL, id, NL, id, EOF
  require_seq(ts, {Tok::Identifier, Tok::NL, Tok::Identifier, Tok::NL, Tok::Identifier, Tok::EOF_});
  auto xs = lexemes(ts);
  REQUIRE(xs[0] == "a");
  REQUIRE(xs[2] == "b");
  REQUIRE(xs[4] == "c");
}

TEST_CASE("identifiers vs keywords")
{
  // Keywords per docs: if, else, sw, case, default, while, do, for, in, try, catch, return
  const char *src = "if else sw case default while do for in try catch return foo bar_123";
  Lexer lx(src);
  auto ts = lx.tokenize();
  require_seq(ts, {Tok::KwIf, Tok::KwElse, Tok::KwSw, Tok::KwCase, Tok::KwDefault,
                   Tok::KwWhile, Tok::KwDo, Tok::KwFor, Tok::KwIn, Tok::KwTry, Tok::KwCatch, Tok::KwReturn,
                   Tok::Identifier, Tok::Identifier, Tok::EOF_});
  auto xs = lexemes(ts);
  REQUIRE(xs[xs.size() - 3] == "foo");
  REQUIRE(xs[xs.size() - 2] == "bar_123");
}

TEST_CASE("integers and floats")
{
  Lexer lx("0 42 001 123456789 3.14 .5 5. .0001 10.0");
  auto ts = lx.tokenize();
  // Int/Float kinds per tokens.hpp; exact set may vary
  require_seq(ts, {Tok::Int, Tok::Int, Tok::Int, Tok::Int,
                   Tok::Float, Tok::Float, Tok::Float, Tok::Float, Tok::Float,
                   Tok::EOF_});
  auto xs = lexemes(ts);
  REQUIRE(xs[0] == "0");
  REQUIRE(xs[1] == "42");
  REQUIRE(xs[4] == "3.14");
  REQUIRE(xs[5] == ".5");
  REQUIRE(xs[6] == "5.");
}

TEST_CASE("strings with escapes and multiline")
{
  const char *src =
      "\"hello\" \"a\\n b\\t c\\\" d\\\\\" \n"
      "\"multi\\nline\"";
  Lexer lx(src);
  auto ts = lx.tokenize();
  // String, String, NL, String, EOF
  require_seq(ts, {Tok::String, Tok::String, Tok::NL, Tok::String, Tok::EOF_});
  auto xs = lexemes(ts);
  REQUIRE(xs[0] == "\"hello\"");
  REQUIRE(xs[1].find("\\n") != std::string::npos);
  REQUIRE(xs[1].find("\\t") != std::string::npos);
  REQUIRE(xs[1].find("\\\"") != std::string::npos);
  REQUIRE(xs[1].find("\\\\") != std::string::npos);
}

TEST_CASE("char literals incl. escapes")
{
  const char *src = "'a' '\\n' '\\''";
  Lexer lx(src);
  auto ts = lx.tokenize();
  require_seq(ts, {Tok::Char, Tok::Char, Tok::Char, Tok::EOF_});
  auto xs = lexemes(ts);
  REQUIRE(xs[0] == "'a'");
  REQUIRE(xs[1] == "'\\n'");
  REQUIRE(xs[2] == "'\\''");
}

TEST_CASE("operators and punctuation")
{
  // Adjust to your tokens.hpp; this suite covers a representative set.
  const char *src =
      "+ - * / % = == != < <= > >= && || ! , . : :: -> { } ( ) [ ]";
  Lexer lx(src);
  auto ts = lx.tokenize();
  // Just ensure we don't misclassify or merge; check first few and last few.
  REQUIRE(ts.front().kind != Tok::EOF_);
  REQUIRE(ts.back().kind == Tok::EOF_);
  // Spot checks for arrow and double-colon
  bool sawArrow = false, sawDblColon = false, sawLBrace = false, sawRBrace = false;
  for (auto &t : ts)
  {
    if (t.lexeme == "->")
      sawArrow = true;
    if (t.lexeme == "::")
      sawDblColon = true;
    if (t.lexeme == "{")
      sawLBrace = true;
    if (t.lexeme == "}")
      sawRBrace = true;
  }
  REQUIRE(sawArrow);
  REQUIRE(sawDblColon);
  REQUIRE(sawLBrace);
  REQUIRE(sawRBrace);
}

TEST_CASE("single-line comments are skipped")
{
  const char *src =
      "a # comment to EOL\n"
      "b # another\n"
      "c";
  Lexer lx(src);
  auto ts = lx.tokenize();
  // a NL b NL c EOF
  require_seq(ts, {Tok::Identifier, Tok::NL, Tok::Identifier, Tok::NL, Tok::Identifier, Tok::EOF_});
  auto xs = lexemes(ts);
  REQUIRE(xs[0] == "a");
  REQUIRE(xs[2] == "b");
  REQUIRE(xs[4] == "c");
}

TEST_CASE("block comments (#* *#) are skipped, can span lines")
{
  const char *src =
      "a #* block \n ignores *# b";
  Lexer lx(src);
  auto ts = lx.tokenize();
  // a WS may become NL depending; minimally ensure a and b are adjacent tokens aside from NL.
  // We only assert presence/order of a, maybe NL, b, EOF.
  REQUIRE(ts.size() >= 3);
  REQUIRE(ts[0].kind == Tok::Identifier);
  REQUIRE(ts[0].lexeme == "a");
  REQUIRE(ts[ts.size() - 2].kind == Tok::Identifier);
  REQUIRE(ts[ts.size() - 2].lexeme == "b");
  REQUIRE(ts.back().kind == Tok::EOF_);
}

TEST_CASE("docstring-style comments are skipped")
{
  const char *src =
      "#*\n"
      " * @author X\n"
      " * @version 1.0\n"
      " *#\n"
      "main() {}";
  Lexer lx(src);
  auto ts = lx.tokenize();
  // Expect 'main ( ) { } EOF' with optional NLs
  bool sawMain = false, sawLParen = false, sawRParen = false, sawLBrace = false, sawRBrace = false;
  for (auto &t : ts)
  {
    if (t.lexeme == "main" && t.kind == Tok::Identifier)
      sawMain = true;
    if (t.lexeme == "(")
      sawLParen = true;
    if (t.lexeme == ")")
      sawRParen = true;
    if (t.lexeme == "{")
      sawLBrace = true;
    if (t.lexeme == "}")
      sawRBrace = true;
  }
  REQUIRE(sawMain);
  REQUIRE(sawLParen);
  REQUIRE(sawRParen);
  REQUIRE(sawLBrace);
  REQUIRE(sawRBrace);
}

TEST_CASE("whitespace is ignored except newlines")
{
  Lexer lx("a     b\t\tc \n d");
  auto ts = lx.tokenize();
  // a b c NL d EOF
  std::vector<Tok> expected = {Tok::Identifier, Tok::Identifier, Tok::Identifier, Tok::NL, Tok::Identifier, Tok::EOF_};
  require_seq(ts, expected);
}

TEST_CASE("peek()/next() contract")
{
  Lexer lx("a b");
  const Token &p1 = lx.peek();
  REQUIRE(p1.kind == Tok::Identifier);
  REQUIRE(p1.lexeme == "a");

  Token n1 = lx.next();
  REQUIRE(n1.kind == Tok::Identifier);
  REQUIRE(n1.lexeme == "a");

  const Token &p2 = lx.peek();
  REQUIRE(p2.kind == Tok::Identifier);
  REQUIRE(p2.lexeme == "b");

  Token n2 = lx.next();
  REQUIRE(n2.kind == Tok::Identifier);
  REQUIRE(n2.lexeme == "b");

  const Token &p3 = lx.peek();
  REQUIRE(p3.kind == Tok::EOF_);
  Token n3 = lx.next();
  REQUIRE(n3.kind == Tok::EOF_);
}

TEST_CASE("mixed realistic snippet tokenizes sanely")
{
  const char *src =
      "struct Vec2 {\n"
      "  i8 x\n"
      "  i8 y\n"
      "}\n"
      "\n"
      "add(i8 a, i8 b) -> i8 {\n"
      "  return a + b\n"
      "}\n";
  Lexer lx(src);
  auto ts = lx.tokenize();
  // Just assert presence and ordering of some key lexemes.
  std::vector<std::string> want = {"struct", "Vec2", "{", "i8", "x", "i8", "y", "}", "add", "(", "i8", "a", ",", "i8", "b", ")", "->", "i8", "{", "return", "a", "+", "b", "}"};
  size_t j = 0;
  for (auto &t : ts)
  {
    if (j < want.size() && t.lexeme == want[j])
      ++j;
  }
  REQUIRE(j == want.size());
}

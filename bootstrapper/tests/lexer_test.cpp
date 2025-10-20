#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "lexer.hpp"

using blam::Lexer;
using blam::Token;
using blam::Tok;
using blam::LexError;
using blam::Pos;
using blam::Range;

static std::vector<Tok> kinds(const std::vector<Token>& ts) {
  std::vector<Tok> ks; ks.reserve(ts.size());
  for (const auto& t : ts) ks.push_back(t.kind);
  return ks;
}

TEST(Lexer, BasicIdentifiersAndKeywords) {
  const std::string src = "if else foo bar return";
  Lexer lx(src);
  auto toks = lx.tokenize();

  ASSERT_GE(toks.size(), 6u);
  EXPECT_EQ(toks[0].kind, Tok::KwIf);
  EXPECT_EQ(toks[1].kind, Tok::KwElse);
  ASSERT_EQ(toks[2].kind, Tok::Ident);
  EXPECT_EQ(toks[2].value, "foo");
  ASSERT_EQ(toks[3].kind, Tok::Ident);
  EXPECT_EQ(toks[3].value, "bar");
  EXPECT_EQ(toks[4].kind, Tok::KwReturn);
  EXPECT_EQ(toks.back().kind, Tok::EOF_);
}

TEST(Lexer, NumbersIntAndFloat) {
  const std::string src = "0 123 3.14";
  Lexer lx(src);
  auto ts = lx.tokenize();

  ASSERT_GE(ts.size(), 4u);
  EXPECT_EQ(ts[0].kind, Tok::Int);   EXPECT_EQ(ts[0].value, "0");
  EXPECT_EQ(ts[1].kind, Tok::Int);   EXPECT_EQ(ts[1].value, "123");
  EXPECT_EQ(ts[2].kind, Tok::Float); EXPECT_EQ(ts[2].value, "3.14");
  EXPECT_EQ(ts.back().kind, Tok::EOF_);
}

TEST(Lexer, OperatorsAndPunctuators) {
  const std::string src = "== <= >= + - * / = < > ( ) { } [ ] , . :";
  Lexer lx(src);
  auto ts = lx.tokenize();

  ASSERT_GE(ts.size(), 20u); // ops + EOF
  EXPECT_EQ(ts[0].kind, Tok::EqEq);
  EXPECT_EQ(ts[1].kind, Tok::Lte);
  EXPECT_EQ(ts[2].kind, Tok::Gte);
  EXPECT_EQ(ts[3].kind, Tok::Plus);
  EXPECT_EQ(ts[4].kind, Tok::Minus);
  EXPECT_EQ(ts[5].kind, Tok::Multiply);
  EXPECT_EQ(ts[6].kind, Tok::Divide);
  EXPECT_EQ(ts[7].kind, Tok::Eq);
  EXPECT_EQ(ts[8].kind, Tok::Lt);
  EXPECT_EQ(ts[9].kind, Tok::Gt);
  EXPECT_EQ(ts[10].kind, Tok::LParen);
  EXPECT_EQ(ts[11].kind, Tok::RParen);
  EXPECT_EQ(ts[12].kind, Tok::LBrace);
  EXPECT_EQ(ts[13].kind, Tok::RBrace);
  EXPECT_EQ(ts[14].kind, Tok::LBracket);
  EXPECT_EQ(ts[15].kind, Tok::RBracket);
  EXPECT_EQ(ts[16].kind, Tok::Comma);
  EXPECT_EQ(ts[17].kind, Tok::Dot);
  EXPECT_EQ(ts[18].kind, Tok::Colon);
  EXPECT_EQ(ts.back().kind, Tok::EOF_);
}

TEST(Lexer, NewlinesAndPositionTracking) {
  const std::string src = "a\nb\r\nc\n";
  Lexer lx(src);
  auto ts = lx.tokenize();

  ASSERT_GE(ts.size(), 7u);
  ASSERT_EQ(ts[0].kind, Tok::Ident); EXPECT_EQ(ts[0].value, "a");
  EXPECT_EQ(ts[1].kind, Tok::NL);
  ASSERT_EQ(ts[2].kind, Tok::Ident); EXPECT_EQ(ts[2].value, "b");
  EXPECT_EQ(ts[3].kind, Tok::NL);
  ASSERT_EQ(ts[4].kind, Tok::Ident); EXPECT_EQ(ts[4].value, "c");
  EXPECT_EQ(ts[5].kind, Tok::NL);
  EXPECT_EQ(ts.back().kind, Tok::EOF_);

  // Spot-check newline token span
  EXPECT_EQ(ts[1].value, "\n");
  EXPECT_EQ(ts[1].range.start.line, 1);
  EXPECT_EQ(ts[1].range.end.line,   2);
}

TEST(Lexer, HorizontalWhitespaceSkipped) {
  const std::string src = "foo\t  \rbar";
  Lexer lx(src);
  auto ts = lx.tokenize();

  ASSERT_GE(ts.size(), 3u);
  ASSERT_EQ(ts[0].kind, Tok::Ident); EXPECT_EQ(ts[0].value, "foo");
  ASSERT_EQ(ts[1].kind, Tok::Ident); EXPECT_EQ(ts[1].value, "bar");
  EXPECT_EQ(ts[2].kind, Tok::EOF_);
}

TEST(Lexer, SingleLineCommentsSkippedByDefault) {
  const std::string src = "x # comment\n y";
  Lexer lx(src);
  auto ts = lx.tokenize();

  ASSERT_GE(ts.size(), 4u);
  auto ks = kinds(ts);
  EXPECT_EQ(ks[0], Tok::Ident);
  EXPECT_EQ(ks[1], Tok::NL);
  EXPECT_EQ(ks[2], Tok::Ident);
  EXPECT_EQ(ts.back().kind, Tok::EOF_);
}

TEST(Lexer, SingleLineCommentsSurfacedWhenEnabled) {
  const std::string src = "x # hello world\ny";
  Lexer lx(src, true);
  auto ts = lx.tokenize();

  ASSERT_GE(ts.size(), 5u);
  EXPECT_EQ(ts[0].kind, Tok::Ident);
  ASSERT_EQ(ts[1].kind, Tok::SlComment);
  EXPECT_EQ(ts[1].value, " hello world"); // no '#', no trailing \n
  EXPECT_EQ(ts[2].kind, Tok::NL);
  EXPECT_EQ(ts[3].kind, Tok::Ident);
  EXPECT_EQ(ts.back().kind, Tok::EOF_);
}

TEST(Lexer, MultiLineCommentsSurfaced) {
  const std::string src = "a #* multi\nline *# b";
  Lexer lx(src, true);
  auto ts = lx.tokenize();

  ASSERT_GE(ts.size(), 4u);
  EXPECT_EQ(ts[0].kind, Tok::Ident);
  ASSERT_EQ(ts[1].kind, Tok::MlComment);
  EXPECT_EQ(ts[1].value, " multi\nline ");
  EXPECT_EQ(ts[2].kind, Tok::Ident);
  EXPECT_EQ(ts.back().kind, Tok::EOF_);
}

TEST(Lexer, MultiLineCommentsSkipped) {
  const std::string src = "a#*c*#b";
  Lexer lx(src);
  auto ts = lx.tokenize();

  ASSERT_GE(ts.size(), 3u);
  ASSERT_EQ(ts[0].kind, Tok::Ident); EXPECT_EQ(ts[0].value, "a");
  ASSERT_EQ(ts[1].kind, Tok::Ident); EXPECT_EQ(ts[1].value, "b");
  EXPECT_EQ(ts.back().kind, Tok::EOF_);
}

TEST(Lexer, MultiLineCommentsUnterminatedThrows) {
  const std::string src = "a #* never ends...";
  Lexer lx(src);
  EXPECT_THROW({ auto _ = lx.tokenize(); (void)_; }, LexError);
}

TEST(Lexer, PeekReturnsNextTokenWithoutConsuming) {
  const std::string src = "foo bar";
  Lexer lx(src);

  const Token& p1 = lx.peek();
  EXPECT_EQ(p1.kind, Tok::Ident);
  EXPECT_EQ(p1.value, "foo");

  Token n1 = lx.next();
  EXPECT_EQ(n1.kind, Tok::Ident);
  EXPECT_EQ(n1.value, "foo");

  Token n2 = lx.next();
  EXPECT_EQ(n2.kind, Tok::Ident);
  EXPECT_EQ(n2.value, "bar");

  Token n3 = lx.next();
  EXPECT_EQ(n3.kind, Tok::EOF_);
}

TEST(Lexer, UnknownCharacterProducesLexError) {
  const std::string src = "foo @ bar";
  Lexer lx(src);
  EXPECT_THROW({ auto _ = lx.tokenize(); (void)_; }, LexError);
}

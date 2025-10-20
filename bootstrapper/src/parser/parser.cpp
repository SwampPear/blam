#include "parser.hpp"
#include "ast.hpp"

namespace blam {

Parser::Parser(std::vector<Token> toks) : toks(std::move(toks)) {
  if (toks.empty() || toks.back().kind != Tok::EOF_) {
    toks.push_back(Token{Tok::EOF_, "", Range{}});
  }
  
  advance();
}

// Internal assertions.
void Parser::assert(bool cond, std::string_view msg) { 
  if (!cond) error_here(msg, cur); 
}

// Advance cursor.
void Parser::advance() {
  if (i < toks.size()) cur = toks[i++];
}

// Peek at next token.
const Token &Parser::peek() {
  static Token eof{Tok::EOF_, "", Range{}};
  if (i < toks.size()) return toks[i];
  return eof;
}

// Compare.
bool Parser::is(Tok k) { 
  return cur.kind == k; 
}

// Match and consume token.
bool Parser::match(Tok k) {
  if (is(k)) {
    advance();
    return true;
  }
  
  return false;
}

 // Expect a token.
void Parser::expect(Tok k, std::string_view msg) {
  if (!match(k))
    error_here(msg, cur);
}

// Obvious.
void Parser::skip_newlines() {
  while (is(Tok::NL))
    advance();
}

// End of statement.
bool Parser::at_stmt_end() { 
  return is(Tok::NL) || is(Tok::RBrace) || is(Tok::EOF_); 
}

// Parse identifier name.
std::string_view Parser::parse_ident_name(std::string_view msg) {
  if (!is(Tok::Ident))
    error_here(msg);

  auto s = cur.value;

  advance();

  return s;
}

// Parse type name.
TypeName Parser::parse_type_name() {
  TypeName tn{};
  tn.name = parse_ident_name();

  return tn;
}

// Parse a top level declaration.
DeclPtr Parser::parse_toplevel() {
  bool isPub = match(Tok::KwPub);
  
  return parse_func(isPub);
}

// Parses a function.
DeclPtr Parser::parse_func(bool isPub) {
  // declaration
  auto f = std::make_shared<FuncDecl>();
  f->isPub = isPub;
  f->name = parse_ident_name("expected function name");

  // parameters
  expect(Tok::LParen, "expected '(' after function name");
  f->params = parse_params();
  expect(Tok::RParen, "expected ')' after parameters");
  if (match(Tok::Arrow))
    f->ret = parse_type_name();
  
  // body
  f->body = parse_block();
  
  return f;
}

// Parse function parameters.
std::vector<Param> Parser::parse_params() {
  std::vector<Param> out;
  
  // immediate end
  if (is(Tok::RParen))
    return out;

  while (true) {
    // name
    Param p{};
    p.name = parse_ident_name("expected parameter name");

    // optional type
    if (match(Tok::Colon))
      p.type = parse_type_name();

    out.push_back(std::move(p));

    // comma
    if (!match(Tok::Comma))
      break;
  }

  // right parentheses
  match(Tok::RParen);

  return out;
}

}  // namespace blam
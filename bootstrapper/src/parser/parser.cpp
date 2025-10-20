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
  expect(Tok::RParen, "Expected ')'");

  return out;
}

StmtPtr Parser::parse_simple_stmt_as_stmt() {
  if (is(Tok::KwConst) || is(Tok::Identifier)) {
    auto save = save_cursor();

    try {
      if (auto vd = parse_vardecl_maybe())
        return *vd;
    } catch  {
      restore_cursor(save);
    }

    save = save_cursor();
    try {
      if (auto as = parse_assign_stmt_maybe())
        return *as;
    } catch {
      restore_cursor(save);
    }
  }

  auto e = parse_expr();
  return std::make_shared<ExprStmt>(e); // ExprStmt has an explicit (ExprPtr) ctor
}

// Parses a scoped block.
std::shared_ptr<BlockStmt> Parser::parse_block() {
  expect(Tok::LBrace, "expected '{' to start block");
  auto b = std::make_shared<BlockStmt>();
  skip_newlines();
  while (!is(Tok::RBrace))
  {
    if (is(Tok::EOF_))
      error_here("unterminated block");
    b->stmts.push_back(parse_stmt());
    skip_newlines();
  }
  expect(Tok::RBrace, "expected '}' to end block");
  return b;
}

StmtPtr Parser::parse_stmt() {
  // return
  if (match(Tok::KwReturn)) {
    auto r = std::make_shared<ReturnStmt>();
    if (!at_stmt_end()) {
      auto e = parse_expr();
      r->value = e;/// also set .value (default is nullptr for bare return)
    }

    if (is(Tok::NL))
      skip_newlines();

    return r;
  }

  // const ident
  if (is(Tok::KwConst) || is(Tok::Ident)) {
    auto save = save_cursor();

    try {
      auto decl = parse_vardecl_maybe();

      if (decl) {
        if (is(Tok::NL))
          skip_newlines();

        return decl;
      }
    } catch (std::runtime_error) {
      restore_cursor(save);
    }

    try {
      auto asg = parse_assign_stmt_maybe();

      if (asg) {
        if (is(Tok::NL))
          skip_newlines();

        return *asg;
      }
    } catch (std::runtime_error) {
      restore_cursor(save);
    }
  }

  // expression statement
  auto e = parse_expr();
  auto es = std::make_shared<ExprStmt>(e);

  if (is(Tok::NL))
    skip_newlines();
    
  return es;
}

}  // namespace blam
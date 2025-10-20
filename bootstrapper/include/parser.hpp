#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>
#include "tokens.hpp"
#include "ast.hpp"

namespace blam {

  // Errors for parsers.
struct ParseError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

// Location message.
std::string make_loc_msg(std::string_view msg, const Token &at) {
  // pos
  const auto line = at.range.start.line;
  const auto col = at.range.start.col;

  // format string
  std::string out;
  out.reserve(msg.size() + 32);
  out.append("L ").append(std::to_string(line)).append(", C ").append(std::to_string(col)).append(": ").append(msg);

  return out;
}

// Safely throw parse error.
[[noreturn]] void error_here(std::string_view msg, Token &cur) {
  throw ParseError(make_loc_msg(msg, cur));
}

// Cursor state.
struct CursorSave {
  size_t i;
  Token cur;
};

class Parser {
public:
  explicit Parser(std::vector<Token> toks);

  std::shared_ptr<Module> parse_module() {
    auto mod = std::make_shared<Module>();
    skip_newlines();
    while (!is(Tok::EOF_))
    {
      mod->decls.push_back(parse_toplevel());
      skip_newlines();
    }
    return mod;
  }

private:
  std::vector<Token> toks;
  size_t i{0};
  Token cur{};

  void assert(bool cond, std::string_view msg) { if (!cond) error_here(msg, cur); }
  void advance();
  const Token &peek();
  bool is(Tok k);

  bool match(Tok k);
  void expect(Tok k, std::string_view msg);

  void skip_newlines();
  bool at_stmt_end();

  std::string_view parse_ident_name(std::string_view msg = "expected identifier");
  TypeName parse_type_name();
  DeclPtr parse_toplevel();

  DeclPtr parse_func(bool isPub);
  std::vector<Param> parse_params();
  std::shared_ptr<BlockStmt> Parser::parse_block();

  StmtPtr parse_stmt();
  StmtPtr parse_simple_stmt_as_stmt();

  CursorSave save_cursor() const { return CursorSave{i, cur}; }
  void restore_cursor(CursorSave s) {
    i = s.i;
    cur = s.cur;
  }

  StmtPtr parse_vardecl_maybe() {
    // optional const
    bool isConst = match(Tok::KwConst);
    if (!isConst && !is(Tok::Ident))
      throw ParseError("expected identifier");

    // type
    std::optional<TypeName> type;
    if (match(Tok::Ident))
      type = parse_type_name();

    // name
    std::string_view name = parse_ident_name("expected variable name");

    // =
    expect(Tok::Eq, "expected '=' in variable declaration");

    // rhs
    auto init = parse_expr();

    auto n = std::make_shared<VarDeclStmt>();
    n->isConst = isConst;
    n->name = std::move(name);
    n->type = type;
    n->init = init;

    return StmtPtr(n);
  }

  std::optional<StmtPtr> parse_assign_stmt_maybe() {
    auto lhsSave = save_cursor();

    auto lhs = parse_lvalue_maybe();
    if (!lhs) {
      restore_cursor(lhsSave);
      return std::nullopt;
    }

    if (!match(Tok::Eq)) {
      restore_cursor(lhsSave);
      return std::nullopt;
    }

    auto rhs = parse_expr();
    auto n = std::make_shared<AssignStmt>();
    n->lhs = *lhs;
    n->rhs = rhs;

    return StmtPtr(n);
  }

  std::optional<ExprPtr> parse_lvalue_maybe() {
    if (!is(Tok::Ident))
      return std::nullopt;

    auto ident = std::make_shared<IdentExpr>();
    ident->value = cur.value;
    advance();

    ExprPtr base = ident;
    while (match(Tok::Dot))
    {
      auto m = std::make_shared<MemberExpr>();
      m->obj = base;
      m->field = parse_ident_name("expected member name after '.'");
      base = m;
    }
    return base;
  }

    // -------- expressions --------
  ExprPtr parse_expr() { 
    return parse_assign_expr(); 
  }

  ExprPtr parse_assign_expr() {
    auto lhsSave = save_cursor();
    auto lhs = parse_lvalue_maybe();
    if (lhs && match(Tok::Eq)) {
      auto n = std::make_shared<BinaryExpr>();
      n->op = "=";
      n->lhs = *lhs;
      n->rhs = parse_assign_expr();

      return n;
    }

    // failure
    restore_cursor(lhsSave);
    return parse_rel();
  }

  ExprPtr parse_rel() {
    auto e = parse_add();
    for (;;) {
      if (match(Tok::Lt))
        e = make_binary("<", e, parse_add());
      else if (match(Tok::Lte))
        e = make_binary("<=", e, parse_add());
      else if (match(Tok::Gt))
        e = make_binary(">", e, parse_add());
      else if (match(Tok::Gte))
        e = make_binary(">=", e, parse_add());
      else
        break;
    }

    return e;
  }

  ExprPtr parse_add() {
    auto e = parse_mul();
    for (;;) {
      if (match(Tok::Plus))
        e = make_binary("+", e, parse_mul());
      else if (match(Tok::Minus))
        e = make_binary("-", e, parse_mul());
      else
        break;
    }
    return e;
  }

  ExprPtr parse_mul() {
    auto e = parse_unary();

    for (;;) {
      if (match(Tok::Multiply))
        e = make_binary("*", e, parse_unary());
      else if (match(Tok::Divide))
        e = make_binary("/", e, parse_unary());
      else
        break;
    }
    return e;
  }

  ExprPtr parse_unary() {
    if (match(Tok::Plus))
    {
      auto u = std::make_shared<UnaryExpr>();
      u->op = "+";
      u->rhs = parse_unary();
      return u;
    }

    if (match(Tok::Minus))
    {
      auto u = std::make_shared<UnaryExpr>();
      u->op = "-";
      u->rhs = parse_unary();
      return u;
    }

    return parse_primary();
  }

  ExprPtr parse_primary() {
    if (is(Tok::Int)) {
      auto n = std::make_shared<IntExpr>();
      n->value = cur.value;
      advance();

      return n;
    }

    if (is(Tok::Float)) {
      auto n = std::make_shared<FloatExpr>();
      n->value = cur.value;
      advance();

      return n;
    }

    if (is(Tok::KwTrue)) {
      auto n = std::make_shared<BoolExpr>();
      n->value = true;
      advance();
      return n;
    }

    if (is(Tok::KwFalse)) {
      auto n = std::make_shared<BoolExpr>();
      n->value = false;
      advance();
      return n;
    }

    if (is(Tok::Ident))
    {
      auto id = std::make_shared<IdentExpr>();
      id->value = cur.value;
      advance();

      return id;
    }

    if (match(Tok::LParen)) {
      auto inside = parse_expr();
      expect(Tok::RParen, "expected ')'");

      return inside;
    }

    error_here("expected expression");
  }

  ExprPtr make_binary(std::string op, ExprPtr a, ExprPtr b) {
    auto n = std::make_shared<BinaryExpr>();

    n->op = std::move(op);
    n->lhs = std::move(a);
    n->rhs = std::move(b);

    return n;
  }
};

} // namespace blam
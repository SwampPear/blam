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
    i_= s.i;
    cur = s.cur;
  }

  StmtPtr parse_vardecl_maybe() {
    // optional const
    bool isConst = match(Tok::KwConst);
    if (!isConst && !is(Tok::Ident))
      throw ParseError("expected identifier");

    // optional type
    std::optional<TypeName> type;
    if (match(Tok::Ident))
      type = parse_type_name();

    std::string_view name = parse_ident_name("expected variable name");
    std::optional<TypeName> ty;
    if (match(Tok::Colon))
      ty = parse_type_name();

    expect(Tok::Eq, "expected '=' in variable declaration");
    auto init = parse_expr();

    auto n = std::make_shared<VarDeclStmt>();
    //n->isConst = isConst;
    n->name = std::move(name);
    n->type = ty;
    n->init = init;

    return StmtPtr(n);
  }

    std::optional<StmtPtr> parse_assign_stmt_maybe()
    {
      auto lhsSave = save_cursor();
      auto lhs = parse_lvalue_maybe();
      if (!lhs)
      {
        restore_cursor(lhsSave);
        return std::nullopt;
      }
      if (!match(Tok::Eq))
      {
        restore_cursor(lhsSave);
        return std::nullopt;
      }
      auto rhs = parse_expr();
      auto n = std::make_shared<AssignStmt>();
      n->lhs = *lhs;
      n->rhs = rhs;
      return StmtPtr(n);
    }

    std::optional<ExprPtr> parse_lvalue_maybe()
    {
      if (!is(Tok::Identifier))
        return std::nullopt;

      auto ident = std::make_shared<IdentExpr>();
      ident->value = cur_.lexeme;
      advance();

      ExprPtr base = ident;
      while (match(Tok::Dot))
      {
        auto m = std::make_shared<MemberExpr>();
        m->obj = base;
        m->field = parse_ident("expected member name after '.'");
        base = m;
      }
      return base;
    }

    // -------- expressions --------
    ExprPtr parse_expr() { return parse_assign_expr(); }

    ExprPtr parse_assign_expr()
    {
      auto lhsSave = save_cursor();
      auto lhs = parse_lvalue_maybe();
      if (lhs && match(Tok::Eq))
      {
        auto n = std::make_shared<BinaryExpr>();
        n->op = "=";
        n->lhs = *lhs;
        n->rhs = parse_assign_expr();
        return n;
      }
      restore_cursor(lhsSave);
      return parse_or();
    }

    ExprPtr parse_or()
    {
      auto e = parse_and();
      while (match(Tok::OrOr))
        e = make_binary("||", e, parse_and());
      return e;
    }

    ExprPtr parse_and()
    {
      auto e = parse_eq();
      while (match(Tok::AndAnd))
        e = make_binary("&&", e, parse_eq());
      return e;
    }

    ExprPtr parse_eq()
    {
      auto e = parse_rel();
      for (;;)
      {
        if (match(Tok::EqEq))
          e = make_binary("==", e, parse_rel());
        else if (match(Tok::BangEq))
          e = make_binary("!=", e, parse_rel());
        else
          break;
      }
      return e;
    }

    ExprPtr parse_rel()
    {
      auto e = parse_add();
      for (;;)
      {
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

    ExprPtr parse_add()
    {
      auto e = parse_mul();
      for (;;)
      {
        if (match(Tok::Plus))
          e = make_binary("+", e, parse_mul());
        else if (match(Tok::Minus))
          e = make_binary("-", e, parse_mul());
        else
          break;
      }
      return e;
    }

    ExprPtr parse_mul()
    {
      auto e = parse_unary();
      for (;;)
      {
        if (match(Tok::Star))
          e = make_binary("*", e, parse_unary());
        else if (match(Tok::Slash))
          e = make_binary("/", e, parse_unary());
        else if (match(Tok::Percent))
          e = make_binary("%", e, parse_unary());
        else
          break;
      }
      return e;
    }

    ExprPtr parse_unary()
    {
      if (match(Tok::Bang))
      {
        auto u = std::make_shared<UnaryExpr>();
        u->op = "!";
        u->rhs = parse_unary();
        return u;
      }
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
      return parse_postfix();
    }

    ExprPtr parse_postfix()
    {
      auto e = parse_primary();
      for (;;)
      {
        if (match(Tok::PlusPlus))
        {
          auto p = std::make_shared<PostfixUpdateExpr>();
          p->op = "++";
          p->target = e;
          e = p;
        }
        else if (match(Tok::MinusMinus))
        {
          auto p = std::make_shared<PostfixUpdateExpr>();
          p->op = "--";
          p->target = e;
          e = p;
        }
        else if (match(Tok::LParen))
        {
          auto c = std::make_shared<CallExpr>();
          c->callee = e;
          if (!is(Tok::RParen))
          {
            for (;;)
            {
              c->args.push_back(parse_expr());
              if (!match(Tok::Comma))
                break;
            }
          }
          expect(Tok::RParen, "expected ')' after arguments");
          e = c;
        }
        else if (match(Tok::Dot))
        {
          auto m = std::make_shared<MemberExpr>();
          m->obj = e;
          m->field = parse_ident("expected member name after '.'");
          e = m;
        }
        else
          break;
      }
      return e;
    }

    ExprPtr parse_primary()
    {
      if (is(Tok::Int))
      {
        auto n = std::make_shared<IntExpr>();
        n->value = cur_.lexeme;
        advance();
        return n;
      }
      if (is(Tok::Float))
      {
        auto n = std::make_shared<FloatExpr>();
        n->value = cur_.lexeme;
        advance();
        return n;
      }
      if (is(Tok::String))
      {
        auto n = std::make_shared<StrExpr>();
        n->value = cur_.lexeme;
        advance();
        return n;
      }
      if (is(Tok::Char))
      { // <-- add char literal support
        auto n = std::make_shared<CharExpr>();
        n->value = cur_.lexeme; // whatever your lexer stores ('Z', escaped, etc.)
        advance();
        return n;
      }
      if (is(Tok::KwTrue))
      {
        auto n = std::make_shared<BoolExpr>();
        n->value = true;
        advance();
        return n;
      }
      if (is(Tok::KwFalse))
      {
        auto n = std::make_shared<BoolExpr>();
        n->value = false;
        advance();
        return n;
      }
      if (is(Tok::Identifier))
      {
        auto id = std::make_shared<IdentExpr>();
        id->value = cur_.lexeme;
        advance();
        return id;
      }
      if (match(Tok::LParen))
      {
        auto inside = parse_expr();
        expect(Tok::RParen, "expected ')'");
        return inside;
      }
      error_here("expected expression");
    }

    ExprPtr make_binary(std::string op, ExprPtr a, ExprPtr b)
    {
      auto n = std::make_shared<BinaryExpr>();
      n->op = std::move(op);
      n->lhs = std::move(a);
      n->rhs = std::move(b);
      return n;
    }
  };

} // namespace blam
*/
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

  // Parses a scoped block.
  std::shared_ptr<BlockStmt> parse_block() {
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

  StmtPtr parse_stmt() {
    // return
    if (match(Tok::KwReturn)) {
      auto r = std::make_shared<ReturnStmt>();
      if (!at_stmt_end())
      {
        auto e = parse_expr();
        r->value = e;/// also set .value (default is nullptr for bare return)
      }
      if (is(Tok::NL))
        skip_newlines();

      return r;
    }

    if (match(Tok::KwBreak)) {
      auto n = std::make_shared<BreakStmt>();
      if (is(Tok::NL))
        skip_newlines();
      return n;
    }

    if (match(Tok::KwContinue)) {
      auto n = std::make_shared<ContinueStmt>();
      if (is(Tok::NL))
        skip_newlines();
      return n;
    }

    if (match(Tok::KwRaise)) {
      auto n = std::make_shared<RaiseStmt>();
      n->value = parse_expr();
      if (is(Tok::NL))
        skip_newlines();
      return n;
    }

    if (match(Tok::KwIf)) {
      auto n = std::make_shared<IfStmt>();
      n->cond = parse_expr();
      n->thenBlock = parse_block();

      skip_newlines();

      if (match(Tok::KwElse)) {
        if (match(Tok::KwIf)) {
          n->elseBlock = std::make_shared<BlockStmt>();
          auto chained = std::make_shared<IfStmt>();
          chained->cond = parse_expr();
          chained->thenBlock = parse_block();
          n->elseBlock->stmts.push_back(chained);
        } else {
          n->elseBlock = parse_block();
        }
      }

      return n;
    }

    if (match(Tok::KwWhile)) {
      auto n = std::make_shared<WhileStmt>();
      n->cond = parse_expr();
      n->body = parse_block();

      return n;
    }

    if (match(Tok::KwDo)) {
      auto n = std::make_shared<DoWhileStmt>();
      n->body = parse_block();
      expect(Tok::KwWhile, "expected 'while' after do-block");
      n->cond = parse_expr();

      if (is(Tok::NL))
        skip_newlines();

      return n;
    }

    if (match(Tok::KwFor)) {
      if (is(Tok::Ident) && peek().kind == Tok::KwIn) {
        auto n = std::make_shared<ForInStmt>();
        n->iter = parse_ident("expected loop variable");
        expect(Tok::KwIn, "expected 'in' in for-in loop");
        n->inExpr = parse_expr();
        n->body = parse_block();
        return n;
      } else {
        auto n = std::make_shared<ForStmt>();
        if (!is(Tok::Comma) && !is(Tok::LBrace))
          n->init = parse_simple_stmt_as_stmt();

        expect(Tok::Comma, "expected ',' after for init");
        if (!is(Tok::Comma) && !is(Tok::LBrace))
          n->cond = parse_expr();

        expect(Tok::Comma, "expected ',' after for condition");
        if (!is(Tok::LBrace))
          n->step = parse_simple_stmt_as_stmt();
        n->body = parse_block();
        return n;
      }
    }

    /*
    if (match(Tok::KwSw)) {
      auto n = std::make_shared<SwitchStmt>();
      n->discr = parse_expr();
      expect(Tok::LBrace, "expected '{' to open switch");
      skip_newlines();
      while (!is(Tok::RBrace))
      {
        if (match(Tok::KwCase))
        {
          SwitchCase c{};
          c.labels.push_back(parse_expr());
          while (match(Tok::Comma))
            c.labels.push_back(parse_expr());
          expect(Tok::Colon, "expected ':' after case labels");
          c.body = parse_block();
          n->cases.push_back(std::move(c));
          skip_newlines();
        }
        else if (match(Tok::KwDefault))
        {
          expect(Tok::Colon, "expected ':' after default");
          n->defaultBody = parse_block();
          skip_newlines();
        }
        else
        {
          error_here("expected 'case' or 'default' in switch");
        }
      }
      expect(Tok::RBrace, "expected '}' to close switch");
      return n;
    }

    if (match(Tok::KwTry))
    {
      auto n = std::make_shared<TryCatchesStmt>();
      n->tryBlk = parse_block();
      skip_newlines();
      bool sawCatch = false;
      while (match(Tok::KwCatch))
      {
        sawCatch = true;
        TypedCatch c{};
        if (is(Tok::Identifier))
          c.typeName = parse_type_name_text(); // optional type
        if (is(Tok::Identifier))
          c.bindName = parse_ident(); // optional name
        c.body = parse_block();
        n->catches.push_back(std::move(c));
        skip_newlines();
      }
      if (!sawCatch)
        error_here("expected at least one 'catch' after try");
      return n;
    }*/

    /*
    if (is(Tok::KwConst) || is(Tok::Identifier))
    {
      auto save = save_cursor();
      try
      {
        auto decl = parse_vardecl_maybe();
        if (decl)
        {
          if (is(Tok::NL))
            skip_newlines();
          return *decl;
        }
      }
      catch (...)
      {
        restore_cursor(save);
      }

      save = save_cursor();
      try
      {
        auto asg = parse_assign_stmt_maybe();
        if (asg)
        {
          if (is(Tok::NL))
            skip_newlines();
          return *asg;
        }
      }
      catch (...)
      {
        restore_cursor(save);
      }
    }

    // Expression statement (uses ExprStmt(ExprPtr) ctor)
    auto e = parse_expr();
    auto es = std::make_shared<ExprStmt>(e);
    if (is(Tok::NL))
      skip_newlines();
    return es;
  }*/

  StmtPtr parse_simple_stmt_as_stmt() {
    if (is(Tok::KwConst) || is(Tok::Identifier)) {
      auto save = save_cursor();
      try {
        if (auto vd = parse_vardecl_maybe())
          return *vd;
      }
      catch {
        restore_cursor(save);
      }

      save = save_cursor();
      try
      {
        if (auto as = parse_assign_stmt_maybe())
          return *as;
      }
      catch (...)
      {
        restore_cursor(save);
      }
    }

    auto e = parse_expr();
    return std::make_shared<ExprStmt>(e); // ExprStmt has an explicit (ExprPtr) ctor
  };

  struct CursorSave {
    size_t i;
    Token cur;
  };
  CursorSave save_cursor() const { return CursorSave{i, cur}; }
  void restore_cursor(CursorSave s) {
    i_= s.i;
    cur = s.cur;
  }

    StmtPtr parse_vardecl_maybe() {
      //bool isConst = match(Tok::KwConst);
      //if (!isConst && !is(Tok::Identifier))
      //  return std::nullopt;

      std::string name = parse_ident("expected variable name");
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
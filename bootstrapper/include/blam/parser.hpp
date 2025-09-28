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

namespace blam
{

  struct ParseError : std::runtime_error
  {
    using std::runtime_error::runtime_error;
  };

  class Parser
  {
  public:
    explicit Parser(std::vector<Token> toks) : toks_(std::move(toks))
    {
      if (toks_.empty() || toks_.back().kind != Tok::EOF_)
      {
        toks_.push_back(Token{Tok::EOF_, "", Range{}});
      }
      advance();
    }

    std::shared_ptr<Module> parse_module()
    {
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
    // -------- state --------
    std::vector<Token> toks_;
    size_t i_{0};
    Token cur_{};

    // -------- error helpers --------
    std::string make_loc_msg(std::string_view msg, const Token &at) const
    {
      const auto line = at.range.start.line;
      const auto col = at.range.start.col;
      std::string out;
      out.reserve(msg.size() + 32);
      out.append("line ").append(std::to_string(line)).append(", col ").append(std::to_string(col)).append(": ").append(msg);
      return out;
    }

    [[noreturn]] void error_here(std::string_view msg) const
    {
      throw ParseError(make_loc_msg(msg, cur_));
    }

    void ensure(bool cond, std::string_view msg)
    {
      if (!cond)
        error_here(msg);
    }

    // -------- cursor --------
    void advance()
    {
      if (i_ < toks_.size())
        cur_ = toks_[i_++];
    }
    const Token &peek() const
    {
      static Token eof{Tok::EOF_, "", Range{}};
      if (i_ < toks_.size())
        return toks_[i_];
      return eof;
    }
    bool is(Tok k) const { return cur_.kind == k; }
    bool match(Tok k)
    {
      if (is(k))
      {
        advance();
        return true;
      }
      return false;
    }
    void expect(Tok k, std::string_view msg)
    {
      if (!match(k))
        error_here(msg);
    }
    void skip_newlines()
    {
      while (is(Tok::NL))
        advance();
    }
    bool at_stmt_end() const { return is(Tok::NL) || is(Tok::RBrace) || is(Tok::EOF_); }

    // -------- small helpers --------
    std::string parse_ident(std::string_view msg = "expected identifier")
    {
      if (!is(Tok::Identifier))
        error_here(msg);
      auto s = cur_.lexeme;
      advance();
      return s;
    }

    // helper (adjust Tok names to match your tokens.hpp)
    bool is_primitive_type_tok(Tok k) const
    {
      switch (k)
      {
      case Tok::KwI8:
      case Tok::KwI16:
      case Tok::KwI32:
      case Tok::KwI64:
      case Tok::KwU8:
      case Tok::KwU16:
      case Tok::KwU32:
      case Tok::KwU64:
      case Tok::KwInt:
      case Tok::KwBool:
      case Tok::KwChar:
      case Tok::KwStr:
      case Tok::KwAny: // dynamic "any"
        return true;
      default:
        return false;
      }
    }

    // type name = primitive | dotted identifiers
    std::string parse_type_name_text()
    {
      // primitives (including 'any') — no dotted qualifiers allowed
      if (is_primitive_type_tok(cur_.kind))
      {
        std::string t = cur_.lexeme; // or map Tok->string if lexeme is empty
        advance();
        return t;
      }

      // user types: Identifier(.Identifier)*
      if (!is(Tok::Identifier))
        error_here("expected type name");
      std::string t = cur_.lexeme;
      advance();
      while (match(Tok::Dot))
      {
        t += ".";
        t += parse_ident("expected identifier after '.'");
      }
      return t;
    }

    TypeName parse_type_name()
    {
      TypeName tn{};
      tn.name = parse_type_name_text();
      return tn;
    }

    // -------- top-level --------
    DeclPtr parse_toplevel()
    {
      bool isPub = match(Tok::KwPub);
      if (match(Tok::KwStruct))
        return parse_struct(isPub);
      return parse_func(isPub);
    }

    DeclPtr parse_struct(bool isPub)
    {
      auto s = std::make_shared<StructDecl>();
      s->isPub = isPub;
      s->name = parse_ident("expected struct name");
      expect(Tok::LBrace, "expected '{'");
      skip_newlines();
      while (!is(Tok::RBrace))
      {
        if (is(Tok::EOF_))
          error_here("unterminated struct body");
        Field f{};
        f.isPub = match(Tok::KwPub); // optional 'pub'
        f.type = parse_type_name();  // type
        f.name = parse_ident("expected field name");
        if (is(Tok::NL))
          skip_newlines();
        else if (!is(Tok::RBrace))
          error_here("expected newline or '}' after field");
      }
      expect(Tok::RBrace, "expected '}'");
      return s;
    }

    DeclPtr parse_func(bool isPub)
    {
      auto f = std::make_shared<FuncDecl>();
      f->isPub = isPub;
      f->name = parse_ident("expected function name");
      expect(Tok::LParen, "expected '(' after function name");
      f->params = parse_params();
      expect(Tok::RParen, "expected ')' after parameters");
      if (match(Tok::Arrow))
        f->ret = parse_type_name();
      f->body = parse_block();
      return f;
    }

    std::vector<Param> parse_params()
    {
      std::vector<Param> out;
      if (is(Tok::RParen))
        return out;
      for (;;)
      {
        Param p{};
        p.name = parse_ident("expected parameter name");
        if (match(Tok::Colon))
          p.type = parse_type_name();
        out.push_back(std::move(p));
        if (!match(Tok::Comma))
          break;
      }
      return out;
    }

    // -------- blocks & statements --------
    std::shared_ptr<BlockStmt> parse_block()
    {
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

    StmtPtr parse_stmt()
    {
      // return
      if (match(Tok::KwReturn))
      {
        auto r = std::make_shared<ReturnStmt>();
        if (!at_stmt_end())
        {
          auto e = parse_expr();
          r->a = e;     // optional field in your AST
          r->value = e; // also set .value (default is nullptr for bare return)
        }
        if (is(Tok::NL))
          skip_newlines();
        return r;
      }

      if (match(Tok::KwBreak))
      {
        auto n = std::make_shared<BreakStmt>();
        if (is(Tok::NL))
          skip_newlines();
        return n;
      }
      if (match(Tok::KwContinue))
      {
        auto n = std::make_shared<ContinueStmt>();
        if (is(Tok::NL))
          skip_newlines();
        return n;
      }

      if (match(Tok::KwRaise))
      {
        auto n = std::make_shared<RaiseStmt>();
        n->value = parse_expr();
        if (is(Tok::NL))
          skip_newlines();
        return n;
      }

      if (match(Tok::KwIf))
      {
        auto n = std::make_shared<IfStmt>();
        n->cond = parse_expr();
        n->thenBlk = parse_block();
        skip_newlines();
        if (match(Tok::KwElse))
        {
          if (match(Tok::KwIf))
          {
            n->elseBlk = std::make_shared<BlockStmt>();
            auto chained = std::make_shared<IfStmt>();
            chained->cond = parse_expr();
            chained->thenBlk = parse_block();
            n->elseBlk->stmts.push_back(chained);
          }
          else
          {
            n->elseBlk = parse_block();
          }
        }
        return n;
      }

      if (match(Tok::KwWhile))
      {
        auto n = std::make_shared<WhileStmt>();
        n->cond = parse_expr();
        n->body = parse_block();
        return n;
      }

      if (match(Tok::KwDo))
      {
        auto n = std::make_shared<DoWhileStmt>();
        n->body = parse_block();
        expect(Tok::KwWhile, "expected 'while' after do-block");
        n->cond = parse_expr();
        if (is(Tok::NL))
          skip_newlines();
        return n;
      }

      if (match(Tok::KwFor))
      {
        if (is(Tok::Identifier) && peek().kind == Tok::KwIn)
        {
          auto n = std::make_shared<ForInStmt>();
          n->iter = parse_ident("expected loop variable");
          expect(Tok::KwIn, "expected 'in' in for-in loop");
          n->inExpr = parse_expr();
          n->body = parse_block();
          return n;
        }
        else
        {
          auto n = std::make_shared<CForStmt>();
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

      if (match(Tok::KwSw))
      {
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
      }

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
    }

    StmtPtr parse_simple_stmt_as_stmt()
    {
      if (is(Tok::KwConst) || is(Tok::Identifier))
      {
        auto save = save_cursor();
        try
        {
          if (auto vd = parse_vardecl_maybe())
            return *vd;
        }
        catch (...)
        {
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
    }

    struct CursorSave
    {
      size_t i;
      Token cur;
    };
    CursorSave save_cursor() const { return CursorSave{i_, cur_}; }
    void restore_cursor(CursorSave s)
    {
      i_ = s.i;
      cur_ = s.cur;
    }

    std::optional<StmtPtr> parse_vardecl_maybe()
    {
      bool isConst = match(Tok::KwConst);
      if (!isConst && !is(Tok::Identifier))
        return std::nullopt;

      std::string name = parse_ident("expected variable name");
      std::optional<TypeName> ty;
      if (match(Tok::Colon))
        ty = parse_type_name();
      expect(Tok::Eq, "expected '=' in variable declaration");
      auto init = parse_expr();

      auto n = std::make_shared<VarDeclStmt>();
      n->isConst = isConst;
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

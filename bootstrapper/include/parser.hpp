#pragma once
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <optional>
#include <memory>
#include "lexer.hpp"
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
    explicit Parser(std::string_view src) : lx_(src) { advance(); }

    std::shared_ptr<Module> parse_module()
    {
      auto mod = std::make_shared<Module>();
      while (!is(Tok::EOF_))
      {
        skip_newlines();
        if (is(Tok::EOF_))
          break;
        mod->decls.push_back(parse_decl());
        skip_newlines();
      }
      return mod;
    }

  private:
    Lexer lx_;
    Token cur_{};

    // ---------- basic helpers ----------
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
    void expect(Tok k, const char *msg)
    {
      if (!is(k))
        throw ParseError(msg);
      advance();
    }
    void advance() { cur_ = lx_.next(); }
    void skip_newlines()
    {
      while (is(Tok::NL))
        advance();
    }

    // ---------- declarations ----------
    DeclPtr parse_decl()
    {
      bool isPub = match(Tok::KwPub);

      if (match(Tok::KwStruct))
        return parse_struct_tail(isPub);
      if (peek_is_func())
        return parse_func_tail(isPub);

      throw ParseError("Expected declaration (struct or function)");
    }

    bool peek_is_func()
    {
      if (cur_.kind != Tok::Identifier)
        return false;
      Token save = cur_;
      advance();
      bool r = is(Tok::LParen);
      cur_ = save;
      return r;
    }

    std::shared_ptr<StructDecl> parse_struct_tail(bool isPub)
    {
      if (!is(Tok::Identifier))
        throw ParseError("Expected struct name");
      auto s = std::make_shared<StructDecl>();
      s->isPub = isPub;
      s->name = cur_.lexeme;
      advance();
      expect(Tok::LBrace, "Expected '{' after struct name");
      skip_newlines();
      while (!is(Tok::RBrace))
      {
        bool fieldPub = match(Tok::KwPub);
        if (!is(Tok::Identifier))
          throw ParseError("Expected field name");
        std::string fname = cur_.lexeme;
        advance();
        expect(Tok::Colon, "Expected ':' after field name");
        TypeName t{parse_type_name()};
        s->fields.push_back(Field{fieldPub, fname, t});
        skip_stmt_terminator();
      }
      advance(); // }
      return s;
    }

    std::shared_ptr<FuncDecl> parse_func_tail(bool isPub)
    {
      auto f = std::make_shared<FuncDecl>();
      f->isPub = isPub;
      if (!is(Tok::Identifier))
        throw ParseError("Expected function name");
      f->name = cur_.lexeme;
      advance();
      expect(Tok::LParen, "Expected '(' after function name");
      if (!is(Tok::RParen))
      {
        for (;;)
        {
          Param p{};
          if (!is(Tok::Identifier))
            throw ParseError("Expected parameter name");
          p.name = cur_.lexeme;
          advance();
          if (match(Tok::Colon))
            p.type = TypeName{parse_type_name()};
          f->params.push_back(std::move(p));
          if (!match(Tok::Comma))
            break;
        }
      }
      expect(Tok::RParen, "Expected ')' after parameters");
      if (match(Tok::Arrow))
        f->ret = TypeName{parse_type_name()};
      f->body = parse_block();
      return f;
    }

    std::string parse_type_name()
    {
      if (!is(Tok::Identifier) &&
          cur_.kind != Tok::KwInt && cur_.kind != Tok::KwUint &&
          cur_.kind != Tok::KwI8 && cur_.kind != Tok::KwI16 && cur_.kind != Tok::KwI32 && cur_.kind != Tok::KwI64 &&
          cur_.kind != Tok::KwU8 && cur_.kind != Tok::KwU16 && cur_.kind != Tok::KwU32 && cur_.kind != Tok::KwU64 &&
          cur_.kind != Tok::KwBool && cur_.kind != Tok::KwChar && cur_.kind != Tok::KwStr && cur_.kind != Tok::KwAny)
      {
        throw ParseError("Expected type name");
      }
      std::string t = cur_.lexeme;
      advance();
      return t;
    }

    // ---------- blocks / statements ----------
    std::shared_ptr<BlockStmt> parse_block()
    {
      expect(Tok::LBrace, "Expected '{' to start block");
      auto blk = std::make_shared<BlockStmt>();
      skip_newlines();
      while (!is(Tok::RBrace))
      {
        blk->stmts.push_back(parse_stmt());
        skip_stmt_terminator();
      }
      advance(); // }
      return blk;
    }

    void skip_stmt_terminator()
    {
      // statements are newline-terminated; allow NL* before closing brace
      while (is(Tok::NL))
        advance();
    }

    // lvalue helpers (Identifier { '.' Identifier })
    bool is_lvalue_start() const { return cur_.kind == Tok::Identifier; }

    ExprPtr parse_lvalue_expr()
    {
      if (cur_.kind != Tok::Identifier)
        throw ParseError("Expected lvalue (identifier)");

      // Start with the base identifier
      ExprPtr lhs = std::make_shared<IdentExpr>();
      static_cast<IdentExpr *>(lhs.get())->value = cur_.lexeme;
      advance(); // consume ident

      // Handle dotted member accesses: a.b.c
      while (match(Tok::Dot))
      {
        if (cur_.kind != Tok::Identifier)
          throw ParseError("Expected member after '.'");

        auto mem = std::make_shared<MemberExpr>();
        mem->obj = lhs;           // previous expression becomes the object
        mem->field = cur_.lexeme; // current identifier is the member name
        advance();                // consume member ident
        lhs = mem;                // upcast to ExprPtr is fine
      }

      return lhs;
    }

    StmtPtr parse_stmt()
    {
      // --- control flow (no parens in headers) ---
      if (match(Tok::KwIf))
        return parse_if();
      if (match(Tok::KwWhile))
        return parse_while();
      if (match(Tok::KwDo))
        return parse_do_while();

      if (match(Tok::KwFor))
      {
        if (cur_.kind == Tok::Identifier)
        {
          Token save = cur_;
          advance();
          bool isForIn = (cur_.kind == Tok::KwIn);
          cur_ = save;
          if (isForIn)
            return parse_for_in();
        }
        return parse_cfor();
      }

      if (match(Tok::KwSw))
        return parse_switch();
      if (match(Tok::KwTry))
        return parse_try_catches();

      // --- returns / flow ---
      if (match(Tok::KwReturn))
      {
        if (is(Tok::NL) || is(Tok::RBrace))
          return std::make_shared<ReturnStmt>();
        auto rs = std::make_shared<ReturnStmt>();
        rs->value = parse_expr();
        return rs;
      }
      if (match(Tok::KwBreak))
        return std::make_shared<BreakStmt>();
      if (match(Tok::KwContinue))
        return std::make_shared<ContinueStmt>();
      if (match(Tok::KwRaise))
      {
        auto r = std::make_shared<RaiseStmt>();
        r->value = parse_expr();
        return r;
      }

      // --- const declaration ---
      if (match(Tok::KwConst))
      {
        auto d = std::make_shared<VarDeclStmt>();
        d->isConst = true;
        if (cur_.kind != Tok::Identifier)
          throw ParseError("Expected name after 'const'");
        d->name = cur_.lexeme;
        advance();
        if (match(Tok::Colon))
          d->type = TypeName{parse_type_name()};
        expect(Tok::Eq, "Expected '=' in const declaration");
        d->init = parse_expr();
        return d;
      }

      // --- mutable decl or assignment or expression-stmt ---
      if (cur_.kind == Tok::Identifier)
      {
        Token save = cur_;
        auto lhs = parse_lvalue_expr();
        if (match(Tok::Colon))
        {
          auto d = std::make_shared<VarDeclStmt>();
          d->isConst = false;
          auto *id = dynamic_cast<IdentExpr *>(lhs.get());
          if (!id)
            throw ParseError("Left of ':' must be identifier");
          d->name = id->value;
          d->type = TypeName{parse_type_name()};
          expect(Tok::Eq, "Expected '=' in variable declaration");
          d->init = parse_expr();
          return d;
        }
        if (match(Tok::Eq))
        {
          auto a = std::make_shared<AssignStmt>();
          a->lhs = lhs;
          a->rhs = parse_expr();
          return a;
        }
        // Not decl/assign: continue parsing expression with existing lhs
        return parse_expr_stmt_with_prefix(lhs);
      }

      // Fallback: plain expression statement
      return std::make_shared<ExprStmt>(parse_expr());
    }

    std::shared_ptr<IfStmt> parse_if()
    {
      auto n = std::make_shared<IfStmt>();
      n->cond = parse_expr();
      n->thenBlk = parse_block();
      if (match(Tok::KwElse))
      {
        if (match(Tok::KwIf))
        {
          auto chain = std::make_shared<BlockStmt>();
          chain->stmts.push_back(parse_if());
          n->elseBlk = chain;
        }
        else
        {
          n->elseBlk = parse_block();
        }
      }
      return n;
    }

    std::shared_ptr<WhileStmt> parse_while()
    {
      auto n = std::make_shared<WhileStmt>();
      n->cond = parse_expr();
      n->body = parse_block();
      return n;
    }

    std::shared_ptr<DoWhileStmt> parse_do_while()
    {
      auto n = std::make_shared<DoWhileStmt>();
      n->body = parse_block();
      expect(Tok::KwWhile, "Expected 'while' after do-block");
      n->cond = parse_expr();
      if (!match(Tok::NL))
        throw ParseError("Expected newline after do-while header");
      return n;
    }

    std::shared_ptr<ForInStmt> parse_for_in()
    {
      auto n = std::make_shared<ForInStmt>();
      if (!is(Tok::Identifier))
        throw ParseError("Expected loop variable name");
      n->iter = cur_.lexeme;
      advance();
      expect(Tok::KwIn, "Expected 'in' in for-loop");
      n->inExpr = parse_expr();
      n->body = parse_block();
      return n;
    }

    std::shared_ptr<CForStmt> parse_cfor()
    {
      auto n = std::make_shared<CForStmt>();

      // init (optional)
      if (!is(Tok::Comma))
      {
        if (match(Tok::KwConst))
        {
          // const decl init
          auto d = std::make_shared<VarDeclStmt>();
          d->isConst = true;
          if (cur_.kind != Tok::Identifier)
            throw ParseError("Expected name after 'const'");
          d->name = cur_.lexeme;
          advance();
          if (match(Tok::Colon))
            d->type = TypeName{parse_type_name()};
          expect(Tok::Eq, "Expected '=' in const declaration");
          d->init = parse_expr();
          n->init = d;
        }
        else if (cur_.kind == Tok::Identifier)
        {
          Token save = cur_;
          auto lhs = parse_lvalue_expr();
          if (match(Tok::Colon))
          {
            auto d = std::make_shared<VarDeclStmt>();
            d->isConst = false;
            auto *id = dynamic_cast<IdentExpr *>(lhs.get());
            if (!id)
              throw ParseError("Left of ':' must be identifier");
            d->name = id->value;
            d->type = TypeName{parse_type_name()};
            expect(Tok::Eq, "Expected '=' in variable declaration");
            d->init = parse_expr();
            n->init = d;
          }
          else if (match(Tok::Eq))
          {
            auto a = std::make_shared<AssignStmt>();
            a->lhs = lhs;
            a->rhs = parse_expr();
            n->init = a;
          }
          else
          {
            n->init = std::make_shared<ExprStmt>(parse_expr_continuation(lhs));
          }
        }
        else
        {
          n->init = std::make_shared<ExprStmt>(parse_expr());
        }
      }
      expect(Tok::Comma, "Expected ',' after for-init");

      // cond (optional)
      if (!is(Tok::Comma))
        n->cond = parse_expr();
      expect(Tok::Comma, "Expected ',' after for-condition");

      // step (optional)
      if (!is(Tok::LBrace))
      {
        if (cur_.kind == Tok::Identifier)
        {
          auto lhs = parse_lvalue_expr();
          if (match(Tok::Eq))
          {
            auto a = std::make_shared<AssignStmt>();
            a->lhs = lhs;
            a->rhs = parse_expr();
            n->step = a;
          }
          else
          {
            n->step = std::make_shared<ExprStmt>(parse_expr_continuation(lhs));
          }
        }
        else
        {
          n->step = std::make_shared<ExprStmt>(parse_expr());
        }
      }

      n->body = parse_block();
      return n;
    }

    std::shared_ptr<SwitchStmt> parse_switch()
    {
      auto n = std::make_shared<SwitchStmt>();
      n->discr = parse_expr();
      expect(Tok::LBrace, "Expected '{' after switch expression");
      skip_newlines();
      while (!is(Tok::RBrace))
      {
        if (match(Tok::KwDefault))
        {
          expect(Tok::Colon, "Expected ':' after 'default'");
          n->defaultBody = parse_block();
          skip_newlines();
          continue;
        }
        if (match(Tok::KwCase))
        {
          SwitchCase c{};
          c.labels.push_back(parse_expr()); // single label for now
          expect(Tok::Colon, "Expected ':' after case label");
          c.body = parse_block();
          n->cases.push_back(std::move(c));
          skip_newlines();
          continue;
        }
        throw ParseError("Expected 'case' or 'default' in switch");
      }
      advance(); // }
      return n;
    }

    StmtPtr parse_try_catches()
    {
      auto n = std::make_shared<TryCatchesStmt>();
      n->tryBlk = parse_block();
      if (!match(Tok::KwCatch))
        throw ParseError("Expected 'catch' after try-block");
      do
      {
        TypedCatch c{};
        // optional type
        if (cur_.kind == Tok::Identifier ||
            cur_.kind == Tok::KwInt || cur_.kind == Tok::KwUint ||
            cur_.kind == Tok::KwI8 || cur_.kind == Tok::KwI16 || cur_.kind == Tok::KwI32 || cur_.kind == Tok::KwI64 ||
            cur_.kind == Tok::KwU8 || cur_.kind == Tok::KwU16 || cur_.kind == Tok::KwU32 || cur_.kind == Tok::KwU64 ||
            cur_.kind == Tok::KwBool || cur_.kind == Tok::KwChar || cur_.kind == Tok::KwStr || cur_.kind == Tok::KwAny)
        {
          c.typeName = cur_.lexeme;
          advance();
        }
        // optional binding name
        if (cur_.kind == Tok::Identifier)
        {
          c.bindName = cur_.lexeme;
          advance();
        }
        c.body = parse_block();
        n->catches.push_back(std::move(c));
      } while (match(Tok::KwCatch));
      return n;
    }

    // ---------- expressions (Pratt) ----------
    int prec_of(const Token &t) const
    {
      switch (t.kind)
      {
      case Tok::OrOr:
        return 1;
      case Tok::AndAnd:
        return 2;
      case Tok::EqEq:
      case Tok::BangEq:
        return 3;
      case Tok::Lt:
      case Tok::Lte:
      case Tok::Gt:
      case Tok::Gte:
        return 4;
      case Tok::Plus:
      case Tok::Minus:
        return 5;
      case Tok::Star:
      case Tok::Slash:
      case Tok::Percent:
        return 6;
      case Tok::Dot:
        return 8; // keep a.b tight
      default:
        return -1;
      }
    }

    StmtPtr parse_expr_stmt_with_prefix(ExprPtr prefix)
    {
      return std::make_shared<ExprStmt>(parse_expr_continuation(prefix));
    }

    ExprPtr parse_expr(int minPrec = 0)
    {
      ExprPtr lhs = parse_prefix();
      return parse_expr_continuation(lhs, minPrec);
    }

    ExprPtr parse_expr_continuation(ExprPtr lhs, int minPrec = 0)
    {
      // postfix chain: calls, member, ++/--
      for (;;)
      {
        if (match(Tok::LParen))
        {
          auto call = std::make_shared<CallExpr>();
          call->callee = lhs;
          if (!is(Tok::RParen))
          {
            for (;;)
            {
              call->args.push_back(parse_expr());
              if (!match(Tok::Comma))
                break;
            }
          }
          expect(Tok::RParen, "Expected ')'");
          lhs = call;
          continue;
        }
        if (match(Tok::Dot))
        {
          if (cur_.kind != Tok::Identifier)
            throw ParseError("Expected member name after '.'");
          auto mem = std::make_shared<MemberExpr>();
          mem->obj = lhs;
          mem->field = cur_.lexeme;
          advance();
          lhs = mem;
          continue;
        }
        if (is(Tok::PlusPlus) || is(Tok::MinusMinus))
        {
          std::string op = cur_.lexeme;
          advance();
          auto up = std::make_shared<PostfixUpdateExpr>();
          up->op = op;
          up->target = lhs;
          lhs = up;
          continue;
        }
        break;
      }

      // infix
      for (;;)
      {
        int p = prec_of(cur_);
        if (p < minPrec)
          break;
        std::string op = cur_.lexeme;
        advance();
        ExprPtr rhs = parse_expr(p + 1);
        auto bin = std::make_shared<BinaryExpr>();
        bin->op = op;
        bin->lhs = lhs;
        bin->rhs = rhs;
        lhs = bin;
      }
      return lhs;
    }

    ExprPtr parse_prefix()
    {
      // unary: '-' '!' only for now
      if (is(Tok::Minus) || is(Tok::Bang))
      {
        std::string op = cur_.lexeme;
        advance();
        auto node = std::make_shared<UnaryExpr>();
        node->op = op;
        node->rhs = parse_prefix();
        return node;
      }

      // literals / identifiers / paren expr
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
        auto n = std::make_shared<IdentExpr>();
        n->value = cur_.lexeme;
        advance();
        return n;
      }

      if (match(Tok::LParen))
      {
        auto e = parse_expr();
        expect(Tok::RParen, "Expected ')' to close parenthesized expression");
        return e;
      }

      throw ParseError("Expected expression");
    }
  };

} // namespace blam

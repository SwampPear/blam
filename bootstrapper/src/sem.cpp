#include "sem.hpp"
#include "ast.hpp" // make sure this provides the AST types used here

#include <sstream>

namespace blam
{

  // ---------- Type ----------
  bool Type::equals(const Type &o) const
  {
    if (kind != o.kind)
      return false;
    switch (kind)
    {
    case TypeKind::Func:
      if (!ret || !o.ret)
        return false;
      if (!ret->equals(*o.ret))
        return false;
      if (params.size() != o.params.size())
        return false;
      for (size_t i = 0; i < params.size(); ++i)
        if (!params[i]->equals(*o.params[i]))
          return false;
      return true;
    case TypeKind::Struct:
      return name == o.name; // nominal
    case TypeKind::Array:
    case TypeKind::Vector:
      if (!elem || !o.elem)
        return false;
      if (!elem->equals(*o.elem))
        return false;
      return fixedLen == o.fixedLen;
    default:
      return true; // primitives/Any/Void/Unknown by kind
    }
  }

  TypePtr Type::prim(TypeKind k)
  {
    Type t;
    t.kind = k;
    return std::make_shared<Type>(std::move(t));
  }
  TypePtr Type::func(std::vector<TypePtr> ps, TypePtr r)
  {
    Type t;
    t.kind = TypeKind::Func;
    t.params = std::move(ps);
    t.ret = std::move(r);
    return std::make_shared<Type>(std::move(t));
  }
  TypePtr Type::named(std::string n)
  {
    Type t;
    t.kind = TypeKind::Struct;
    t.name = std::move(n);
    return std::make_shared<Type>(std::move(t));
  }
  TypePtr Type::array(TypePtr e, std::optional<size_t> n)
  {
    Type t;
    t.kind = n ? TypeKind::Array : TypeKind::Vector;
    t.elem = std::move(e);
    t.fixedLen = n;
    return std::make_shared<Type>(std::move(t));
  }

  // ---------- Scope ----------
  Symbol *Scope::lookupLocal(std::string_view n)
  {
    auto it = table.find(std::string(n));
    return it == table.end() ? nullptr : &it->second;
  }
  Symbol *Scope::lookup(std::string_view n)
  {
    for (Scope *s = this; s; s = s->parent)
    {
      if (auto *hit = s->lookupLocal(n))
        return hit;
    }
    return nullptr;
  }
  Symbol &Scope::insertOrMergeFunc(const Symbol &s)
  {
    auto [it, fresh] = table.emplace(s.name, s);
    if (fresh)
      return it->second;

    Symbol &existing = it->second;
    if (existing.kind == SymKind::Func && s.kind == SymKind::Func)
    {
      for (auto &ov : s.overloads)
        existing.overloads.push_back(ov);
      return existing;
    }
    throw SemError("redeclaration of '" + s.name + "'");
  }

  // ---------- TypeEnv ----------
  TypeEnv::TypeEnv()
  {
    using K = TypeKind;
    named["i8"] = Type::prim(K::I8);
    named["i16"] = Type::prim(K::I16);
    named["i32"] = Type::prim(K::I32);
    named["i64"] = Type::prim(K::I64);
    named["u8"] = Type::prim(K::U8);
    named["u16"] = Type::prim(K::U16);
    named["u32"] = Type::prim(K::U32);
    named["u64"] = Type::prim(K::U64);
    named["bool"] = Type::prim(K::Bool);
    named["char"] = Type::prim(K::Char);
    named["str"] = Type::prim(K::Str);
    named["any"] = Type::prim(K::Any);
    named["void"] = Type::prim(K::Void);
  }
  TypePtr TypeEnv::resolveName(const std::string &n) const
  {
    auto it = named.find(n);
    return it == named.end() ? nullptr : it->second;
  }

  // ---------- SemAnalyzer ----------
  void SemAnalyzer::analyze(const std::shared_ptr<Module> &mod)
  {
    if (!mod)
      return;
    collectDecls(*mod);       // gather structs & funcs into globals (with empty overload sigs)
    buildFunctionTypes(*mod); // compute concrete function types for overloads
    checkBodies(*mod);        // typecheck statements/expressions
  }

  // Phase 1: collect top-level names
  void SemAnalyzer::collectDecls(Module &m)
  {
    for (auto &dptr : m.decls)
    {
      if (auto *s = dynamic_cast<StructDecl *>(dptr.get()))
      {
        // nominal struct types
        tenv_.named[s->name] = Type::named(s->name);

        Symbol sym;
        sym.kind = SymKind::Struct;
        sym.name = s->name;
        sym.type = tenv_.named[s->name];
        sym.ast = s;
        // insert (no overloads for structs)
        auto [it, fresh] = global_.table.emplace(sym.name, sym);
        if (!fresh)
          throw SemError("redeclaration of struct '" + sym.name + "'");
        continue;
      }

      if (auto *f = dynamic_cast<FuncDecl *>(dptr.get()))
      {
        Symbol::Overload ovl{f, nullptr /*to be filled in phase 2*/};

        Symbol sym;
        sym.kind = SymKind::Func;
        sym.name = f->name;
        sym.ast = f;
        sym.overloads.push_back(ovl);

        global_.insertOrMergeFunc(sym);
        continue;
      }
    }
  }

  // Phase 2: compute function types from params/return
  void SemAnalyzer::buildFunctionTypes(Module &m)
  {
    auto toType = [&](const std::optional<std::string> &maybeName) -> TypePtr
    {
      if (!maybeName.has_value())
        return tenv_.resolveName("any"); // weak typing default
      auto t = tenv_.resolveName(*maybeName);
      if (!t)
        throw SemError("unknown type name: " + *maybeName);
      return t;
    };

    for (auto &kv : global_.table)
    {
      Symbol &sym = kv.second;
      if (sym.kind != SymKind::Func)
        continue;

      for (auto &ovl : sym.overloads)
      {
        FuncDecl *f = ovl.f;
        std::vector<TypePtr> params;
        params.reserve(f->params.size());
        for (auto &p : f->params)
        {
          // Param::typeName should be optional<string> or similar; adapt if different
          std::optional<std::string> tn;
          if (p.type.has_value())
            tn = p.type->name; // TypeName{name:string}
          params.push_back(toType(tn));
        }
        std::optional<std::string> retName;
        if (f->ret.has_value())
          retName = f->ret->name;
        TypePtr ret = f->ret.has_value() ? toType(retName) : tenv_.resolveName("void");
        ovl.sig = Type::func(std::move(params), ret);
      }
    }
  }

  // Phase 3: typecheck function bodies
  void SemAnalyzer::checkBodies(Module &m)
  {
    for (auto &kv : global_.table)
    {
      Symbol &sym = kv.second;
      if (sym.kind != SymKind::Func)
        continue;
      for (auto &ovl : sym.overloads)
      {
        checkFunction(ovl.f, sym, ovl);
      }
    }
  }

  void SemAnalyzer::checkFunction(FuncDecl *f, const Symbol &fnSym, const Symbol::Overload &ovl)
  {
    // Prepare function scope with params
    Scope fnScope;
    fnScope.parent = &global_;

    // bind params
    for (size_t i = 0; i < f->params.size(); ++i)
    {
      Symbol v;
      v.kind = SymKind::Var;
      v.name = f->params[i].name;
      v.type = ovl.sig->params[i];
      v.ast = nullptr;
      auto [it, fresh] = fnScope.table.emplace(v.name, v);
      if (!fresh)
        throw SemError("duplicate parameter name '" + v.name + "' in function '" + f->name + "'");
    }

    // Check body
    std::optional<TypePtr> expectedRet;
    if (ovl.sig->ret->kind != TypeKind::Void)
      expectedRet = ovl.sig->ret;

    if (f->body)
    {
      checkBlock(f->body.get(), fnScope, expectedRet);
    }
    else
    {
      // No body: allowed for extern/builtins; nothing else to do.
    }

    // Special rule: main must be void per docs
    if (f->name == "main" && ovl.sig->ret->kind != TypeKind::Void)
    {
      throw SemError("main cannot return a value");
    }
  }

  void SemAnalyzer::checkBlock(BlockStmt *b, Scope &scope, std::optional<TypePtr> expectedReturn)
  {
    Scope inner;
    inner.parent = &scope;
    for (auto &st : b->stmts)
    {
      checkStmt(st, inner, expectedReturn);
    }
  }

  void SemAnalyzer::checkStmt(const StmtPtr &s, Scope &scope, std::optional<TypePtr> expectedReturn)
  {
    if (!s)
      return;

    if (auto *es = dynamic_cast<ExprStmt *>(s.get()))
    {
      (void)typeOf(es->expr, scope);
      return;
    }

    if (auto *rs = dynamic_cast<ReturnStmt *>(s.get()))
    {
      if (!expectedReturn.has_value())
      {
        // function is void; return must not carry a value
        if (rs->value)
          throw SemError("void function cannot return a value");
        return;
      }
      // has expected return type
      if (!rs->value)
        throw SemError("non-void function must return a value");
      TypePtr vt = typeOf(rs->value, scope);
      requireAssignable(vt, *expectedReturn, "return");
      return;
    }

    // TODO: add variable declarations, if/switch/loops etc. as your AST grows.
  }

  // ---------- Expression typing ----------
  TypePtr SemAnalyzer::typeOf(const ExprPtr &e, Scope &scope)
  {
    if (!e)
      return tenv_.resolveName("void");

    if (auto *id = dynamic_cast<IdentExpr *>(e.get()))
      return typeOfIdent(id, scope);
    if (auto *se = dynamic_cast<StrExpr *>(e.get()))
      return tenv_.resolveName("str");
    if (auto *ie = dynamic_cast<IntExpr *>(e.get()))
      return tenv_.resolveName("i32"); // simple default
    if (auto *be = dynamic_cast<BoolExpr *>(e.get()))
      return tenv_.resolveName("bool");
    if (auto *ce = dynamic_cast<CharExpr *>(e.get()))
      return tenv_.resolveName("char");
    if (auto *fe = dynamic_cast<FloatExpr *>(e.get()))
      return tenv_.resolveName("i32"); // adjust when float type exists
    if (auto *call = dynamic_cast<CallExpr *>(e.get()))
      return typeOfCall(call, scope);

    // TODO: BinaryExpr, UnaryExpr etc.
    return tenv_.resolveName("any");
  }

  TypePtr SemAnalyzer::typeOfIdent(IdentExpr *e, Scope &scope)
  {
    auto *sym = scope.lookup(e->value);
    if (!sym)
      throw SemError("unknown identifier: " + e->value);

    if (sym->kind == SymKind::Var || sym->kind == SymKind::Struct)
    {
      if (!sym->type)
        return tenv_.resolveName("any");
      return sym->type;
    }
    if (sym->kind == SymKind::Func)
    {
      // Bare function identifier evaluates to a (set of) function(s).
      // If single overload, return its function type; else 'any' for now.
      if (sym->overloads.size() == 1 && sym->overloads[0].sig)
        return sym->overloads[0].sig;
      return tenv_.resolveName("any");
    }
    return tenv_.resolveName("any");
  }

  TypePtr SemAnalyzer::typeOfCall(CallExpr *e, Scope &scope)
  {
    // Callee must be an identifier for now
    auto *ident = dynamic_cast<IdentExpr *>(e->callee.get());
    if (!ident)
    {
      // Future: callable values
      throw SemError("unsupported call target");
    }
    auto *sym = scope.lookup(ident->value);
    if (!sym || sym->kind != SymKind::Func)
    {
      throw SemError("call to unknown function '" + ident->value + "'");
    }

    // Try overload resolution by arity + assignability
    std::vector<TypePtr> argTypes;
    argTypes.reserve(e->args.size());
    for (auto &a : e->args)
      argTypes.push_back(typeOf(a, scope));

    for (auto &ovl : sym->overloads)
    {
      auto sig = ovl.sig;
      if (!sig || sig->kind != TypeKind::Func)
        continue;
      if (sig->params.size() != argTypes.size())
        continue;

      bool ok = true;
      for (size_t i = 0; i < argTypes.size(); ++i)
      {
        if (!isAssignableTo(argTypes[i], sig->params[i]))
        {
          ok = false;
          break;
        }
      }
      if (ok)
        return sig->ret;
    }

    // No exact match; make a helpful message
    std::ostringstream msg;
    msg << "no matching overload for call to '" << ident->value << "' with (";
    for (size_t i = 0; i < argTypes.size(); ++i)
    {
      if (i)
        msg << ", ";
      msg << (int)argTypes[i]->kind;
    }
    msg << ")";
    throw SemError(msg.str());
  }

  // ---------- Helpers ----------
  bool SemAnalyzer::isAssignableTo(const TypePtr &from, const TypePtr &to)
  {
    if (!from || !to)
      return true;
    if (to->kind == TypeKind::Any)
      return true;
    if (from->kind == TypeKind::Any)
      return true;
    if (from->equals(*to))
      return true;

    // trivial numeric widening: treat all ints as i32 for now
    if (isNumeric(from->kind) && isNumeric(to->kind))
      return true;

    return false;
  }

  bool SemAnalyzer::isNumeric(TypeKind k)
  {
    using K = TypeKind;
    switch (k)
    {
    case K::I8:
    case K::I16:
    case K::I32:
    case K::I64:
    case K::U8:
    case K::U16:
    case K::U32:
    case K::U64:
      return true;
    default:
      return false;
    }
  }

  TypePtr SemAnalyzer::widenNumeric(const TypePtr &a, const TypePtr &b)
  {
    // Placeholder: bias to i32 for arithmetic
    (void)b;
    return Type::prim(TypeKind::I32);
  }

  void SemAnalyzer::requireAssignable(const TypePtr &from, const TypePtr &to, const std::string &ctx)
  {
    if (!isAssignableTo(from, to))
    {
      std::ostringstream ss;
      ss << "type mismatch in " << ctx << ": cannot assign ";
      ss << (int)from->kind << " to " << (int)to->kind;
      throw SemError(ss.str());
    }
  }

  void SemAnalyzer::requireVoidReturnAllowed(std::optional<TypePtr> expected, const std::string &where)
  {
    if (expected.has_value() && (*expected)->kind != TypeKind::Void)
    {
      throw SemError(where + ": expected a value");
    }
  }

} // namespace blam

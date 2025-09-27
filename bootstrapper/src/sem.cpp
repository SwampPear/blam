#include "sem.hpp"
#include <sstream>

namespace blam
{

  // ------------------------ Type --------------------------
  bool Type::equals(const Type &o) const
  {
    if (kind != o.kind)
      return false;
    switch (kind)
    {
    case TypeKind::Func:
      if (!ret || !o.ret || !ret->equals(*o.ret) || params.size() != o.params.size())
        return false;
      for (size_t i = 0; i < params.size(); ++i)
        if (!params[i]->equals(*o.params[i]))
          return false;
      return true;
    case TypeKind::Struct:
      return name == o.name; // nominal
    case TypeKind::Array:
    case TypeKind::Vector:
      if (!elem || !o.elem || !elem->equals(*o.elem))
        return false;
      return fixedLen == o.fixedLen;
    default:
      return true; // primitives/Any/Void/Unknown compare by kind
    }
  }

  TypePtr Type::prim(TypeKind k) { return std::make_shared<Type>(Type{k}); }

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

  // ------------------------ Scope -------------------------
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
  Symbol &Scope::insertOrThrow(const Symbol &s)
  {
    auto [it, ok] = table.emplace(s.name, s);
    if (!ok)
    {
      // Overloads allowed only for functions
      Symbol &existing = it->second;
      if (s.kind == SymKind::Func && existing.kind == SymKind::Func)
      {
        // append overload sigs
        for (auto &sig : s.overloadSigs)
          existing.overloadSigs.push_back(sig);
        return existing;
      }
      throw SemError(s.where, "redeclaration of '" + s.name + "'");
    }
    return it->second;
  }

  // ------------------------ TypeEnv -----------------------
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
    if (it == named.end())
      return nullptr;
    return it->second;
  }

  // ---------------------- SemAnalyzer ---------------------
  void SemAnalyzer::analyze(const std::shared_ptr<Module> &mod)
  {
    if (!mod)
      return;
    collectDecls(*mod);
    resolveAndType(*mod);
  }

  void SemAnalyzer::collectDecls(Module &m)
  {
    // Phase 1: collect all top-level symbols
    for (auto &dptr : m.decls)
    {
      if (auto *s = dptr->asStruct())
        collectStructDecl(*s, global_);
      else if (auto *f = dptr->asFunc())
        collectFuncDecl(*f, global_);
      else
      {
        // add more kinds as you add language features
      }
    }
  }

  void SemAnalyzer::resolveAndType(Module &m)
  {
    for (auto &dptr : m.decls)
    {
      if (auto *s = dptr->asStruct())
        resolveStruct(*s, global_);
      else if (auto *f = dptr->asFunc())
        resolveFunc(*f, global_);
    }
  }

  void SemAnalyzer::collectStructDecl(StructDecl &s, Scope &sc)
  {
    // Register nominal type
    auto t = Type::named(s.name);
    tenv_.named[s.name] = t;

    // Insert struct symbol
    Symbol sym{SymKind::Struct, s.name, s.where, /*overloads*/ {}, t, &s};
    sc.insertOrThrow(sym);
  }

  void SemAnalyzer::resolveStruct(StructDecl &s, Scope &sc)
  {
    // Create a child scope for fields/method prototypes if needed
    Scope fields{.parent = &sc};
    for (auto &fld : s.fields)
    {
      TypePtr ft = resolveTypeName(fld.type);
      Symbol sym{SymKind::Field, fld.name, fld.where, {}, ft, &fld};
      fields.insertOrThrow(sym);
    }

    // Register methods as function overloads with receiver type
    if (!s.methods.empty())
    {
      for (auto &m : s.methods)
      {
        // method signature: (self, params...) -> ret
        std::vector<TypePtr> params;
        params.push_back(tenv_.named[s.name]); // receiver (nominal)
        for (auto &p : m.params)
        {
          TypePtr pt = p.type.has_value() ? resolveTypeName(*p.type)
                                          : Type::prim(TypeKind::Any);
          params.push_back(pt);
        }
        TypePtr rt = m.ret.has_value() ? resolveTypeName(*m.ret)
                                       : Type::prim(TypeKind::Void);
        auto sig = Type::func(std::move(params), rt);

        Symbol fun{SymKind::Func, m.name, m.where, {sig}, /*type*/ {}, &m};
        sc.insertOrThrow(fun); // overload-friendly
      }
    }
  }

  void SemAnalyzer::collectFuncDecl(FuncDecl &f, Scope &sc)
  {
    // Build function type from header only
    std::vector<TypePtr> params;
    for (auto &p : f.params)
    {
      TypePtr pt = p.type.has_value() ? resolveTypeName(*p.type)
                                      : Type::prim(TypeKind::Any);
      params.push_back(pt);
    }
    TypePtr rt = f.ret.has_value() ? resolveTypeName(*f.ret)
                                   : Type::prim(TypeKind::Void);
    auto sig = Type::func(std::move(params), rt);

    Symbol fun{SymKind::Func, f.name, f.where, {sig}, /*type*/ {}, &f};
    sc.insertOrThrow(fun);
  }

  void SemAnalyzer::resolveFunc(FuncDecl &f, Scope &sc)
  {
    // Establish function scope
    Scope fnScope{.parent = &sc};

    // Bind parameters
    for (auto &p : f.params)
    {
      TypePtr pt = p.type.has_value() ? resolveTypeName(*p.type)
                                      : Type::prim(TypeKind::Any);
      Symbol s{SymKind::Param, p.name, p.where, {}, pt, &p};
      fnScope.insertOrThrow(s);
    }

    // Body
    std::optional<TypePtr> expectedRet =
        f.ret.has_value() ? std::optional<TypePtr>(resolveTypeName(*f.ret))
                          : std::optional<TypePtr>(Type::prim(TypeKind::Void));

    resolveStmt(*f.body, fnScope, expectedRet);
  }

  void SemAnalyzer::resolveStmt(Stmt &st, Scope &sc, std::optional<TypePtr> expectedRet)
  {
    switch (st.tag())
    {
    case StmtTag::Block:
    {
      Scope inner{.parent = &sc};
      for (auto &s : st.asBlock()->stmts)
        resolveStmt(*s, inner, expectedRet);
      break;
    }
    case StmtTag::VarDecl:
    {
      auto *vd = st.asVarDecl();
      TypePtr t = vd->type.has_value() ? resolveTypeName(*vd->type)
                                       : Type::prim(TypeKind::Unknown);
      if (vd->init)
      {
        TypePtr initT = resolveExpr(*vd->init, sc);
        if (t->kind == TypeKind::Unknown)
          t = initT;
        else if (!unify(t, initT))
          semError(vd->where, "type mismatch in initialization");
      }
      Symbol sym{SymKind::Var, vd->name, vd->where, {}, t, vd};
      sc.insertOrThrow(sym);
      break;
    }
    case StmtTag::Assign:
    {
      auto *as = st.asAssign();
      TypePtr lt = resolveExpr(*as->lhs, sc);
      TypePtr rt = resolveExpr(*as->rhs, sc);
      if (!unify(lt, rt))
        semError(as->where, "type mismatch in assignment");
      break;
    }
    case StmtTag::ExprStmt:
    {
      (void)resolveExpr(*st.asExprStmt()->expr, sc);
      break;
    }
    case StmtTag::If:
    {
      auto *iff = st.asIf();
      TypePtr ct = resolveExpr(*iff->cond, sc);
      if (ct->kind != TypeKind::Bool && ct->kind != TypeKind::Any)
        semError(iff->cond->where, "if condition must be bool");
      resolveStmt(*iff->thenBlk, sc, expectedRet);
      if (iff->elseBlk)
        resolveStmt(*iff->elseBlk, sc, expectedRet);
      break;
    }
    case StmtTag::While:
    {
      auto *w = st.asWhile();
      TypePtr ct = resolveExpr(*w->cond, sc);
      if (ct->kind != TypeKind::Bool && ct->kind != TypeKind::Any)
        semError(w->cond->where, "while condition must be bool");
      resolveStmt(*w->body, sc, expectedRet);
      break;
    }
    case StmtTag::Return:
    {
      auto *r = st.asReturn();
      TypePtr got = r->expr ? resolveExpr(*r->expr, sc) : Type::prim(TypeKind::Void);
      if (expectedRet && !unify(*expectedRet, got))
        semError(r->where, "return type does not match function return type");
      break;
    }
    default:
      // Add: for, switch, try/catch, break/continue (validate context), etc.
      break;
    }
  }

  TypePtr SemAnalyzer::resolveExpr(Expr &e, Scope &sc)
  {
    switch (e.tag())
    {
    case ExprTag::IntLit:
      return Type::prim(TypeKind::I32); // refine later
    case ExprTag::FloatLit:
      return Type::prim(TypeKind::I64); // placeholder if you have float types
    case ExprTag::BoolLit:
      return Type::prim(TypeKind::Bool);
    case ExprTag::CharLit:
      return Type::prim(TypeKind::Char);
    case ExprTag::StrLit:
      return Type::prim(TypeKind::Str);

    case ExprTag::Name:
    {
      auto

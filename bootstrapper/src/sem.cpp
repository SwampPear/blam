#include "sem.hpp"

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
      for (auto &sig : s.overloadSigs)
        existing.overloadSigs.push_back(sig);
      return existing;
    }
    // otherwise it's a redeclaration
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
    collectDecls(*mod);
  }

  void SemAnalyzer::collectDecls(Module &m)
  {
    for (auto &dptr : m.decls)
    {
      if (auto *s = dynamic_cast<StructDecl *>(dptr.get()))
      {
        tenv_.named[s->name] = Type::named(s->name);

        Symbol sym;
        sym.kind = SymKind::Struct;
        sym.name = s->name;
        sym.type = tenv_.named[s->name];
        sym.ast = s;
        global_.insertOrMergeFunc(sym);
        continue;
      }

      if (auto *f = dynamic_cast<FuncDecl *>(dptr.get()))
      {
        Symbol sym;
        sym.kind = SymKind::Func;
        sym.name = f->name;
        sym.ast = f;
        // (Optionally build Type::func from params/ret if present)
        global_.insertOrMergeFunc(sym);
        continue;
      }

      // Add other Decl kinds as your AST grows.
    }
  }

} // namespace blam

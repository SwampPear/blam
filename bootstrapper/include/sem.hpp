#pragma once
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "ast.hpp"
#include "tokens.hpp" // for Range

namespace blam
{

  // ---------- Errors ----------
  struct SemError : std::runtime_error
  {
    Range where{};
    explicit SemError(const std::string &msg) : std::runtime_error(msg) {}
    SemError(const Range &r, const std::string &msg) : std::runtime_error(msg), where(r) {}
  };

  // ---------- Types ----------
  enum class TypeKind
  {
    I8,
    I16,
    I32,
    I64,
    U8,
    U16,
    U32,
    U64,
    Bool,
    Char,
    Str,
    Any,
    Void,
    Func,
    Struct,
    Vector,
    Array,
    Union,
    Intersect,
    Unknown
  };

  struct Type;
  using TypePtr = std::shared_ptr<Type>;

  struct Type
  {
    TypeKind kind{TypeKind::Unknown};
    std::vector<TypePtr> params;    // for Func
    TypePtr ret;                    // for Func
    std::string name;               // for Struct (nominal)
    TypePtr elem;                   // for Array/Vector
    std::optional<size_t> fixedLen; // for Array

    bool equals(const Type &other) const;
    static TypePtr prim(TypeKind k);
    static TypePtr func(std::vector<TypePtr> ps, TypePtr r);
    static TypePtr named(std::string n);
    static TypePtr array(TypePtr e, std::optional<size_t> n);
  };

  // ---------- Symbols & Scopes ----------
  enum class SymKind
  {
    Var,
    Func,
    Struct,
    Param,
    Field
  };

  struct Symbol
  {
    SymKind kind{SymKind::Var};
    std::string name;
    std::vector<TypePtr> overloadSigs; // for functions
    TypePtr type;                      // for non-functions
    Node *ast{nullptr};                // optional backref
  };

  struct Scope
  {
    Scope *parent{nullptr};
    std::unordered_map<std::string, Symbol> table;

    Symbol *lookupLocal(std::string_view n);
    Symbol *lookup(std::string_view n);
    Symbol &insertOrMergeFunc(const Symbol &s); // allow func overloads, reject other redecls
  };

  // ---------- Type Environment ----------
  struct TypeEnv
  {
    std::unordered_map<std::string, TypePtr> named;
    TypeEnv(); // fills primitives

    TypePtr resolveName(const std::string &n) const;
  };

  // ---------- Semantic Analyzer ----------
  class SemAnalyzer
  {
  public:
    explicit SemAnalyzer(TypeEnv env = TypeEnv{}) : tenv_(std::move(env)) {}
    void analyze(const std::shared_ptr<Module> &mod);

    Scope *global() { return &global_; }
    const TypeEnv &types() const { return tenv_; }

  private:
    TypeEnv tenv_;
    Scope global_{};

    void collectDecls(Module &m); // phase 1: index top-level decls
  };

} // namespace blam

/*
#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <unordered_map>
#include <stdexcept>

#include "blam/tokens.hpp"

namespace blam {
  struct Module;
  struct Decl;
  struct StructDecl;
  struct FuncDecl;
  struct BlockStmt;
  struct Stmt;
  struct Expr;
  struct ReturnStmt;
  struct ExprStmt;
  struct IdentExpr;
  struct CallExpr;
  struct IntExpr;
  struct BoolExpr;
  struct CharExpr;
  struct StrExpr;
  struct FloatExpr;
}

namespace blam {

  using ExprPtr = std::shared_ptr<Expr>;
  using StmtPtr = std::shared_ptr<Stmt>;

  // -------- Types --------
  enum class TypeKind
  {
    Unknown,
    Any,
    Void,
    // primitives
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
    // composites
    Func,
    Struct,
    Array, // fixed length
    Vector // growable
  };

  struct Type;
  using TypePtr = std::shared_ptr<Type>;

  struct Type
  {
    TypeKind kind{TypeKind::Unknown};

    // func
    std::vector<TypePtr> params;
    TypePtr ret;

    // named/struct
    std::string name;

    // array/vector
    TypePtr elem;
    std::optional<size_t> fixedLen; // present => Array, absent => Vector

    bool equals(const Type &o) const;

    static TypePtr prim(TypeKind k);
    static TypePtr func(std::vector<TypePtr> ps, TypePtr r);
    static TypePtr named(std::string n);
    static TypePtr array(TypePtr e, std::optional<size_t> n = std::nullopt);
  };

  // -------- Symbols / Scopes --------
  enum class SymKind
  {
    Var,
    Func,
    Struct
  };

  struct Symbol
  {
    SymKind kind{SymKind::Var};
    std::string name;
    TypePtr type; // for Var/Struct; for Func this may be null if using overloads
    // Overloads: each entry corresponds to a concrete function decl + its func type
    struct Overload
    {
      FuncDecl *f;
      TypePtr sig;
    };
    std::vector<Overload> overloads;
    void *ast{nullptr}; // optional backref
  };

  struct Scope
  {
    Scope *parent{nullptr};
    std::unordered_map<std::string, Symbol> table;

    Symbol *lookupLocal(std::string_view n);
    Symbol *lookup(std::string_view n);

    // insert func; merges overloads
    Symbol &insertOrMergeFunc(const Symbol &s);
    // insert non-func (var/struct); rejects redecl
    Symbol &insert(const Symbol &s)
    {
      auto [it, fresh] = table.emplace(s.name, s);
      if (!fresh)
        throw std::runtime_error("redeclaration of '" + s.name + "'");
      return it->second;
    }
  };

  // -------- Type Environment --------
  struct TypeEnv
  {
    std::unordered_map<std::string, TypePtr> named;
    TypeEnv();
    TypePtr resolveName(const std::string &n) const;
  };

  // -------- Semantic Analyzer --------
  struct SemError : std::runtime_error
  {
    Range where{};
    explicit SemError(const std::string &msg) : std::runtime_error(msg) {}
    SemError(std::string msg, Range r) : std::runtime_error(std::move(msg)), where(r) {}
  };

  class SemAnalyzer
  {
  public:
    void analyze(const std::shared_ptr<Module> &mod);

    // After analyze:
    const Scope &globals() const { return global_; }
    const TypeEnv &tenv() const { return tenv_; }

  private:
    TypeEnv tenv_;
    Scope global_{};

    // Phases
    void collectDecls(Module &m);
    void buildFunctionTypes(Module &m); // attach concrete function types to overloads
    void checkBodies(Module &m);

    // Function / block / stmt
    void checkFunction(FuncDecl *f, const Symbol &fnSym, const Symbol::Overload &ovl);
    void checkBlock(BlockStmt *b, Scope &scope, std::optional<TypePtr> expectedReturn);
    void checkStmt(const StmtPtr &s, Scope &scope, std::optional<TypePtr> expectedReturn);

    // Expr typing
    TypePtr typeOf(const ExprPtr &e, Scope &scope);
    TypePtr typeOfIdent(IdentExpr *e, Scope &scope);
    TypePtr typeOfCall(CallExpr *e, Scope &scope);

    // Helpers
    static bool isAssignableTo(const TypePtr &from, const TypePtr &to);
    static bool isNumeric(TypeKind k);
    static TypePtr widenNumeric(const TypePtr &a, const TypePtr &b); // trivial int32 bias
    static void requireAssignable(const TypePtr &from, const TypePtr &to, const std::string &ctx);
    static void requireVoidReturnAllowed(std::optional<TypePtr> expected, const std::string &where);
  };

} // namespace blam
*/
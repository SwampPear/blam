#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>

// These come from your project:
#include "ast.hpp" // Module, Decl/Stmt/Expr nodes, Range, etc.
#include "tokens.hpp"

namespace blam
{

  // ------------------------ Errors ------------------------
  struct SemError : std::runtime_error
  {
    Range where{};
    SemError(const Range &r, const std::string &msg) : std::runtime_error(msg), where(r) {}
  };

  // ------------------------ Types -------------------------
  enum class TypeKind
  {
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
    Any,
    Void,
    // constructed
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

    // Function type: (params...) -> ret
    std::vector<TypePtr> params;
    TypePtr ret{};

    // Named/nominal types (e.g., struct Foo)
    std::string name;

    // Array/Vector/etc.
    TypePtr elem;
    std::optional<size_t> fixedLen;

    // For quick equality (nominal for structs, structural for funcs/prims)
    bool equals(const Type &other) const;

    static TypePtr prim(TypeKind k);
    static TypePtr func(std::vector<TypePtr> ps, TypePtr r);
    static TypePtr named(std::string n);
    static TypePtr array(TypePtr e, std::optional<size_t> n);
  };

  // --------------------- Symbols & Scopes -----------------
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
    SymKind kind;
    std::string name;
    Range where;
    // Overloads (for functions): same name, different signature
    std::vector<TypePtr> overloadSigs;
    // Resolved type (vars/params/fields/structs)
    TypePtr type;
    // Backref to AST, if helpful
    Node *ast{nullptr};
  };

  struct Scope
  {
    Scope *parent{nullptr};
    std::unordered_map<std::string, Symbol> table;

    Symbol *lookupLocal(std::string_view n);
    Symbol *lookup(std::string_view n);
    Symbol &insertOrThrow(const Symbol &s);
  };

  // --------------------- Type Environment -----------------
  struct TypeEnv
  {
    // nominal type registry (structs, aliases)
    std::unordered_map<std::string, TypePtr> named;
    // predefined primitives
    TypeEnv();

    TypePtr resolveName(const std::string &n) const; // returns nullptr if missing
  };

  // --------------------- Semantic Analyzer ----------------
  class SemAnalyzer
  {
  public:
    explicit SemAnalyzer(TypeEnv env = TypeEnv{}) : tenv_(std::move(env)) {}

    // Entry point
    void analyze(const std::shared_ptr<Module> &mod);

    // After analyze(), you can inspect symbol tables, etc.
    Scope *global() { return &global_; }
    const TypeEnv &types() const { return tenv_; }

  private:
    TypeEnv tenv_;
    Scope global_{};

    // Pass 1: collect top-level decls
    void collectDecls(Module &m);

    // Pass 2: resolve/validate inside decls
    void resolveAndType(Module &m);

    // Helpers
    void collectStructDecl(StructDecl &s, Scope &sc);
    void resolveStruct(StructDecl &s, Scope &sc);

    void collectFuncDecl(FuncDecl &f, Scope &sc);
    void resolveFunc(FuncDecl &f, Scope &sc);

    // Statements/Expressions
    void resolveStmt(Stmt &st, Scope &sc, std::optional<TypePtr> expectedRet);
    TypePtr resolveExpr(Expr &e, Scope &sc);

    // Type resolution from AST TypeName (handles `any`, primitives, dotted names)
    TypePtr resolveTypeName(const TypeName &tn);

    // Function call/type checking
    TypePtr checkCall(const Range &callWhere,
                      const std::string &calleeName,
                      const std::vector<TypePtr> &argTypes,
                      const std::vector<TypePtr> &candidateSigs);

    // Unification (sketch; extend as needed)
    bool unify(const TypePtr &a, const TypePtr &b);

    // Error utilities
    [[noreturn]] void semError(const Range &r, std::string msg);
  };

} // namespace blam

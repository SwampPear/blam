#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace blam
{
  // Basic syntax node.
  struct Node
  {
    virtual ~Node() = default;
  };

  // Variable type names.
  struct TypeName
  {
    std::string name;
  };

  // Basic expression.
  struct Expr : Node
  {
  };
  using ExprPtr = std::shared_ptr<Expr>;

  // Identifiers like variable names, function names, etc.
  struct IdentExpr : Expr
  {
    std::string value;
  };

  // Non-decimal numerics.
  struct IntExpr : Expr
  {
    std::string value;
  };

  // Decimal numerics.
  struct FloatExpr : Expr
  {
    std::string value;
  };

  // Strings.
  struct StrExpr : Expr
  {
    std::string value;
  };

  // True/False.
  struct BoolExpr : Expr
  {
    bool value{false};
  };

  // For negative numbers, negations, e.t.c.
  struct UnaryExpr : Expr
  {
    std::string op; // operator text, e.g. "-" or "!"
    ExprPtr rhs;    // operand
  };

  // For arithmetic/boolean expressions
  struct BinaryExpr : Expr
  {
    std::string op; // operator text, e.g. "+", "=="
    ExprPtr lhs, rhs;
  };

  // Function call.
  struct CallExpr : Expr
  {
    ExprPtr callee;            // thing being called
    std::vector<ExprPtr> args; // positional args
  };

  // Member access (a.b).
  struct MemberExpr : Expr
  {
    ExprPtr obj;       // base expression
    std::string field; // member name
  };

  // Postfix update: x++, x--
  struct PostfixUpdateExpr : Expr
  {
    std::string op; // "++" or "--"
    ExprPtr target; // IdentExpr | MemberExpr
  };

  // Basic statement.
  struct Stmt : Node
  {
  };
  using StmtPtr = std::shared_ptr<Stmt>;

  // Expression as a statement.
  struct ExprStmt : Stmt
  {
    ExprPtr expr;

    explicit ExprStmt(ExprPtr e) : expr(std::move(e)) {}
  };

  // Return statement.
  struct ReturnStmt : Stmt
  {
    std::optional<ExprPtr> a;
    ExprPtr value; // missing => bare 'return'
  };

  // Breaks a loop.
  struct BreakStmt : Stmt
  {
  };

  // Continues a loop.
  struct ContinueStmt : Stmt
  {
  };

  // Raises an error.
  struct RaiseStmt : Stmt
  {
    ExprPtr value;
  };

  // { stmt* }
  struct BlockStmt : Stmt
  {
    std::vector<StmtPtr> stmts;
  };

  // if cond { ... } [else { ... } | else if ...]
  struct IfStmt : Stmt
  {
    ExprPtr cond;
    std::shared_ptr<BlockStmt> thenBlk;
    std::shared_ptr<BlockStmt> elseBlk; // may be null
  };

  // while cond { ... }
  struct WhileStmt : Stmt
  {
    ExprPtr cond;
    std::shared_ptr<BlockStmt> body;
  };

  // for x in expr { ... }
  struct ForInStmt : Stmt
  {
    std::string iter; // iterator variable
    ExprPtr inExpr;   // collection/expression
    std::shared_ptr<BlockStmt> body;
  };

  // do { ... } while cond
  struct DoWhileStmt : Stmt
  {
    std::shared_ptr<BlockStmt> body;
    ExprPtr cond;
  };

  // for init, cond, step { ... }
  struct CForStmt : Stmt
  {
    std::optional<StmtPtr> init; // VarDeclStmt | AssignStmt | ExprStmt | null
    std::optional<ExprPtr> cond; // optional loop condition
    std::optional<StmtPtr> step; // AssignStmt | ExprStmt | null
    std::shared_ptr<BlockStmt> body;
  };

  // One switch case.
  struct SwitchCase
  {
    std::vector<ExprPtr> labels; // typically a single literal/expression
    std::shared_ptr<BlockStmt> body;
  };

  // sw expr { case X: { ... } default: { ... } }
  struct SwitchStmt : Stmt
  {
    ExprPtr discr; // discriminant expression
    std::vector<SwitchCase> cases;
    std::shared_ptr<BlockStmt> defaultBody; // may be null
  };

  // catch part: catch [Type] [name] { ... }
  struct TypedCatch
  {
    std::optional<std::string> typeName; // written type name; resolved later
    std::optional<std::string> bindName; // bound identifier
    std::shared_ptr<BlockStmt> body;
  };

  // try { ... } catch ... { ... } ...
  struct TryCatchesStmt : Stmt
  {
    std::shared_ptr<BlockStmt> tryBlk;
    std::vector<TypedCatch> catches; // one or more
  };

  // const/var declaration.
  struct VarDeclStmt : Stmt
  {
    bool isConst{false};          // true for 'const'
    std::optional<TypeName> type; // optional annotation
    std::string name;
    ExprPtr init; // initializer
  };

  // Assignment: lhs = rhs
  struct AssignStmt : Stmt
  {
    ExprPtr lhs; // IdentExpr | MemberExpr (LValue)
    ExprPtr rhs;
  };

  // Top-level declaration.
  struct Decl : Node
  {
  };
  using DeclPtr = std::shared_ptr<Decl>;

  // Function parameter.
  struct Param
  {
    std::string name;
    std::optional<TypeName> type; // optional param type
  };

  // Function definition.
  struct FuncDecl : Decl
  {
    bool isPub{false};
    std::string name;
    std::vector<Param> params;
    std::optional<TypeName> ret; // present if "-> Type" was provided
    std::shared_ptr<BlockStmt> body;
  };

  // Struct field.
  struct Field
  {
    bool isPub{false};
    std::string name;
    TypeName type;
  };

  // Struct definition.
  struct StructDecl : Decl
  {
    bool isPub{false};
    std::string name;
    std::vector<Field> fields;
  };

  // Compilation unit.
  struct Module : Node
  {
    std::vector<DeclPtr> decls;
  };

} // namespace blam

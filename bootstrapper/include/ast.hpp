#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace blam {
  // basic syntax node
  struct Node { virtual ~Node() = default; };

  // basic expression
  struct Expr : Node {};
  using ExprPtr = std::shared_ptr<Expr>;

  struct TypeName {
    std::string name;
  };

  struct IdentExpr : Expr {
    std::string value;
  };

  struct IntExpr : Expr { // non-decimal
    std::string value;
  };

  struct FloatExpr : Expr { // decimal
    std::string value;
  };

  struct StrExpr : Expr {
    std::string value;
  };

  struct BoolExpr : Expr {
    bool value{false};
  };

  struct UnaryExpr : Expr {
    std::string op;
    ExprPtr rhs;
  };

  struct BinaryExpr : Expr {
    std::string op;
    ExprPtr lhs, rhs;
  };

  struct CharExpr : Expr {
    std::string value;
  };

  struct CallExpr : Expr {
    ExprPtr callee;
    std::vector<ExprPtr> args;
  };

  struct MemberExpr : Expr { // for member access
    ExprPtr obj;       // base expression
    std::string field; // member name
  };

  struct PostfixUpdateExpr : Expr {
    std::string op; // "++" or "--"
    ExprPtr target; // IdentExpr | MemberExpr
  };

  // basic statement
  struct Stmt : Node {};
  using StmtPtr = std::shared_ptr<Stmt>;

  struct ExprStmt : Stmt {
    ExprPtr expr;
    explicit ExprStmt(ExprPtr e) : expr(std::move(e)) {}
  };

  struct ReturnStmt : Stmt {
    std::shared_ptr<ExprPtr> value; // missing => bare 'return'
  };

  struct BreakStmt : Stmt {};
  struct ContinueStmt : Stmt {};

  struct RaiseStmt : Stmt {
    ExprPtr value;
  };

  struct BlockStmt : Stmt { // { stmt* }
    std::vector<StmtPtr> stmts;
  };
  using BlockStmtPtr = std::shared_ptr<BlockStmt>;

  struct IfStmt : Stmt {
    ExprPtr cond;
    BlockStmtPtr thenBlock;
    BlockStmtPtr elseBlock;
  };

  struct WhileStmt : Stmt {
    ExprPtr cond;
    std::shared_ptr<BlockStmt> body;
  };

  struct ForInStmt : Stmt {
    std::string iter;  // variable
    ExprPtr inExpr;    // collection/expression
    BlockStmtPtr body;
  };

  // do { ... } while cond
  struct DoWhileStmt : Stmt {
    ExprPtr cond;
    BlockStmtPtr body;
  };

  struct ForStmt : Stmt {
    std::shared_ptr<Stmt> init;    // VarDeclStmt | AssignStmt | ExprStmt | null
    std::shared_ptr<ExprPtr> cond; // optional loop condition
    std::optional<StmtPtr> step;   // AssignStmt | ExprStmt | null
    BlockStmtPtr body;
  };

  // One switch case.
  struct SwitchCase {
    std::vector<ExprPtr> labels;
    BlockStmtPtr body;
  };

  // sw expr { case X: { ... } default: { ... } }
  struct SwitchStmt : Stmt {
    ExprPtr discr; // discriminant expression
    std::vector<SwitchCase> cases;
    BlockStmtPtr defaultBody; // may be null
  };

  // catch part: catch [Type] [name] { ... }
  struct TypedCatch {
    std::optional<std::string> typeName; // written type name; resolved later
    std::optional<std::string> bindName; // bound identifier
    std::shared_ptr<BlockStmt> body;
  };

  struct TryCatchesStmt : Stmt {
    BlockStmtPtr tryBlk;
    std::vector<TypedCatch> catches; // one or more
  };

  struct VarDeclStmt : Stmt {
    bool isConst{false};
    bool isPublic{true};

    std::optional<TypeName> type;

    std::string name;
    ExprPtr init; // initializer
  };

  // top leve declaration
  struct Decl : Node {};
  using DeclPtr = std::shared_ptr<Decl>;

  struct Param {
    std::string name;
    std::optional<TypeName> type; // optional param type
  };

  struct FuncDecl : Decl {
    bool isPub{false};
    std::string name;
    std::vector<Param> params;
    std::optional<TypeName> ret;
    std::shared_ptr<BlockStmt> body;
  };

  /*
  struct Field {
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
  };*/
} // namespace blam

from __future__ import annotations
from dataclasses import dataclass, field
from typing import List, Optional

class ASTNode: pass
class Expr(ASTNode): pass

@dataclass(slots=True)
class NumberExpr(Expr):
    value: float

@dataclass(slots=True)
class VariableExpr(Expr):
    name: str

@dataclass(slots=True)
class BinaryExpr(Expr):
    op: str
    lhs: Expr
    rhs: Expr

@dataclass(slots=True)
class CallExpr(Expr):
    callee: str
    args: List[Expr] = field(default_factory=list)

# -------- Statements --------
class Stmt(ASTNode): pass

@dataclass(slots=True)
class Block(Stmt):
    body: List[Stmt]

@dataclass(slots=True)
class VarDecl(Stmt):
    name: str
    init: Optional[Expr]  # let x = expr; init may be None

@dataclass(slots=True)
class Assign(Stmt):
    name: str
    value: Expr

@dataclass(slots=True)
class IfStmt(Stmt):
    cond: Expr
    then_branch: Block
    elifs: List[tuple[Expr, Block]] = field(default_factory=list)
    else_branch: Optional[Block] = None

@dataclass(slots=True)
class WhileStmt(Stmt):
    cond: Expr
    body: Block

@dataclass(slots=True)
class ReturnStmt(Stmt):
    value: Optional[Expr] = None

@dataclass(slots=True)
class FuncDef(Stmt):
    name: str
    params: List[str]
    body: Block

@dataclass(slots=True)
class ExprStmt(Stmt):
    expr: Expr

# A whole program: list of top-level statements
@dataclass(slots=True)
class Program(ASTNode):
    body: List[Stmt]

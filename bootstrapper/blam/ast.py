# blam/ast.py
from __future__ import annotations

from dataclasses import dataclass, field
from typing import List


class ASTNode:
    """Base AST node."""
    pass


class Expr(ASTNode):
    """Base expression node."""
    pass


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


__all__ = [
    "ASTNode",
    "Expr",
    "NumberExpr",
    "VariableExpr",
    "BinaryExpr",
    "CallExpr",
]

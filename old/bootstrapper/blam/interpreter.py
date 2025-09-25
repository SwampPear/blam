from __future__ import annotations
from dataclasses import dataclass
from typing import Any, Dict, Optional, List

from blam.ast import (
    Program, Stmt, Block, VarDecl, Assign, IfStmt, WhileStmt, ReturnStmt,
    FuncDef, ExprStmt,
    Expr, NumberExpr, VariableExpr, BinaryExpr, CallExpr
)

# --- Runtime support ---------------------------------------------------------

@dataclass
class Function:
    name: str
    params: List[str]
    body: Block
    env: "Env"  # closure

class ReturnSignal(Exception):
    def __init__(self, value: Any):
        self.value = value

class Env:
    def __init__(self, parent: Optional["Env"]=None):
        self.parent = parent
        self.table: Dict[str, Any] = {}

    def get(self, name: str) -> Any:
        if name in self.table:
            return self.table[name]
        if self.parent:
            return self.parent.get(name)
        raise RuntimeError(f"Undefined name '{name}'")

    def set(self, name: str, value: Any) -> None:
        # Assign into the nearest scope that has the name; define locally if missing
        if name in self.table:
            self.table[name] = value
        elif self.parent and self.parent.has(name):
            self.parent.set(name, value)
        else:
            self.table[name] = value

    def has(self, name: str) -> bool:
        return name in self.table or (self.parent.has(name) if self.parent else False)

    def define(self, name: str, value: Any) -> None:
        self.table[name] = value


# --- Interpreter -------------------------------------------------------------

class Interpreter:
    def __init__(self):
        self.global_env = Env()
        # minimal stdlib
        self.global_env.define("print", lambda *xs: print(*xs))

    # Entry point
    def run(self, program: Program) -> Any:
        return self.exec_block(Block(program.body), self.global_env)

    # ----- statements

    def exec_block(self, block: Block, env: Env) -> None:
        local = Env(env)
        for st in block.body:
            self.exec_stmt(st, local)

    def exec_stmt(self, st: Stmt, env: Env) -> None:
        if isinstance(st, Block):
            self.exec_block(st, env)
            return
        if isinstance(st, VarDecl):
            val = self.eval_expr(st.init, env) if st.init else None
            env.define(st.name, val)
            return
        if isinstance(st, Assign):
            val = self.eval_expr(st.value, env)
            env.set(st.name, val)
            return
        if isinstance(st, IfStmt):
            if self.truthy(self.eval_expr(st.cond, env)):
                self.exec_block(st.then_branch, env)
                return
            for cond, blk in st.elifs:
                if self.truthy(self.eval_expr(cond, env)):
                    self.exec_block(blk, env)
                    return
            if st.else_branch:
                self.exec_block(st.else_branch, env)
            return
        if isinstance(st, WhileStmt):
            while self.truthy(self.eval_expr(st.cond, env)):
                self.exec_block(st.body, env)
            return
        if isinstance(st, ReturnStmt):
            val = self.eval_expr(st.value, env) if st.value else None
            raise ReturnSignal(val)
        if isinstance(st, FuncDef):
            fn = Function(st.name, st.params, st.body, env)
            env.define(st.name, fn)
            return
        if isinstance(st, ExprStmt):
            self.eval_expr(st.expr, env)
            return
        raise RuntimeError(f"Unknown statement: {st}")

    # ----- expressions

    def eval_expr(self, e: Optional[Expr], env: Env) -> Any:
        if e is None:
            return None
        if isinstance(e, NumberExpr):
            return e.value
        if isinstance(e, VariableExpr):
            return env.get(e.name)
        if isinstance(e, BinaryExpr):
            l = self.eval_expr(e.lhs, env)
            r = self.eval_expr(e.rhs, env)
            op = e.op
            if   op == "+":   return l + r
            elif op == "-":   return l - r
            elif op == "*":   return l * r
            elif op == "/":   return l / r
            elif op == "**":  return l ** r
            elif op == "==":  return 1.0 if l == r else 0.0
            elif op == "!=":  return 1.0 if l != r else 0.0
            elif op == "<":   return 1.0 if l <  r else 0.0
            elif op == "<=":  return 1.0 if l <= r else 0.0
            elif op == ">":   return 1.0 if l >  r else 0.0
            elif op == ">=":  return 1.0 if l >= r else 0.0
            else:
                raise RuntimeError(f"Unknown operator {op}")
        if isinstance(e, CallExpr):
            callee = env.get(e.callee)
            args = [self.eval_expr(a, env) for a in e.args]
            # builtin
            if callable(callee) and not isinstance(callee, Function):
                return callee(*args)
            # user-defined
            if isinstance(callee, Function):
                call_env = Env(callee.env)
                if len(args) != len(callee.params):
                    raise RuntimeError(f"Arity mismatch for {callee.name}: expected {len(callee.params)}, got {len(args)}")
                for k, v in zip(callee.params, args):
                    call_env.define(k, v)
                try:
                    self.exec_block(callee.body, call_env)
                except ReturnSignal as rs:
                    return rs.value
                return None
            raise RuntimeError(f"Not callable: {e.callee}")
        raise RuntimeError(f"Unknown expr: {e}")

    @staticmethod
    def truthy(v: Any) -> bool:
        if v is None: return False
        if isinstance(v, (int, float)): return v != 0
        return bool(v)

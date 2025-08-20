from __future__ import annotations

from typing import List, Optional

from blam.ast import Expr, NumberExpr, VariableExpr, BinaryExpr
from blam.llist import LListNode
from blam.token import Token, Type, type_to_string


class Parser:
    def __init__(self, head: Optional[LListNode[Token]], src: str) -> None:
        self.curr: Optional[LListNode[Token]] = head
        self.src = src

    # ------------------------ helpers ------------------------
    def peek(self) -> Token:
        if not self.curr or not self.curr.data:
            raise RuntimeError("Unexpected end of token stream.")
        return self.curr.data

    def skip_whitespace(self) -> None:
        # Treat RAW and NLINE as skippable, same as C++ version.
        while self.curr and self.curr.data and (
            self.curr.data.type in (Type.WHITESPACE, Type.RAW, Type.NLINE)
        ):
            self.curr = self.curr.next

    def advance(self) -> None:
        if self.curr:
            self.curr = self.curr.next
        self.skip_whitespace()

    def match(self, t: Type) -> bool:
        self.skip_whitespace()
        if self.curr and self.curr.data.type == t:
            self.advance()
            return True
        return False

    # -------- precedence & associativity (same values as C++) --------

    def get_precedence(self, t: Type) -> int:
        if t in (Type.EQEQ, Type.NEQ):
            return 5
        if t in (Type.LT, Type.LTE, Type.GT, Type.GTE):
            return 10
        if t in (Type.PLUS, Type.MIN):
            return 20
        if t in (Type.MULT, Type.DIV):
            return 30
        if t == Type.EXP:
            return 40
        return -1

    def is_right_associative(self, t: Type) -> bool:
        return t == Type.EXP

    # ------------------------ recursive descent ------------------------

    def parse(self) -> List[Expr]:
        ast: List[Expr] = []
        self.skip_whitespace()
        while self.curr and self.curr.data and self.curr.data.type != Type.END:
            ast.append(self.parse_expression())
        return ast

    def parse_expression(self) -> Expr:
        lhs = self.parse_primary()
        return self.parse_binary_op_rhs(0, lhs)

    def parse_primary(self) -> Expr:
        if not self.curr or not self.curr.data:
            raise RuntimeError("Unexpected end of input to primary expression.")

        tok = self.peek()

        if tok.type == Type.IDENT:
            name = self.src[tok.pos : tok.pos + tok.length]
            self.advance()
            return VariableExpr(name)

        if tok.type in (Type.DECIMAL, Type.NUMBER):
            # parse numbers (integers and decimals) as float
            value_str = self.src[tok.pos : tok.pos + tok.length]
            value = float(value_str)
            self.advance()
            return NumberExpr(value)

        if tok.type == Type.SMBRACKET_L:
            self.advance()  # consume '('
            expr = self.parse_expression()
            if not self.match(Type.SMBRACKET_R):
                raise RuntimeError("Expected closing ')'")
            return expr

        raise RuntimeError(f"Unknown token in expression: {type_to_string(tok.type)}")

    def parse_binary_op_rhs(self, min_prec: int, lhs: Expr) -> Expr:
        while True:
            # ensure we're looking at a real operator
            self.skip_whitespace()

            op_type = self.curr.data.type if (self.curr and self.curr.data) else Type.END
            op_prec = self.get_precedence(op_type)
            if op_prec < min_prec:
                break

            # capture operator lexeme from source
            tok = self.curr.data
            op_symbol = self.src[tok.pos : tok.pos + tok.length]

            # consume operator
            self.curr = self.curr.next

            # parse RHS primary
            self.skip_whitespace()
            rhs = self.parse_primary()

            # see if next operator binds tighter (or equal & right-assoc)
            while True:
                self.skip_whitespace()
                next_type = self.curr.data.type if (self.curr and self.curr.data) else Type.END
                next_prec = self.get_precedence(next_type)
                if next_prec < 0:
                    break

                if op_prec < next_prec or (op_prec == next_prec and self.is_right_associative(op_type)):
                    next_min = op_prec + (0 if self.is_right_associative(op_type) else 1)
                    rhs = self.parse_binary_op_rhs(next_min, rhs)
                break

            # accumulate
            lhs = BinaryExpr(op_symbol, lhs, rhs)

        return lhs

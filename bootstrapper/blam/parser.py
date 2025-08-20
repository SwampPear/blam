from __future__ import annotations
from typing import List, Optional

from blam.ast import (
    Expr, NumberExpr, VariableExpr, BinaryExpr, CallExpr,
    Stmt, Block, VarDecl, Assign, IfStmt, WhileStmt, ReturnStmt, FuncDef, ExprStmt, Program
)
from blam.llist import LListNode
from blam.token import Token, Type, type_to_string

class Parser:
    def __init__(self, head: Optional[LListNode[Token]], src: str) -> None:
        self.curr: Optional[LListNode[Token]] = head
        self.src = src

    def peek(self) -> Token:
        if not self.curr or not self.curr.data:
            raise RuntimeError("Unexpected end of token stream.")
        return self.curr.data

    def skip_whitespace(self) -> None:
        while self.curr and self.curr.data and self.curr.data.type in (Type.WHITESPACE, Type.RAW, Type.NLINE):
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

    def expect(self, t: Type, msg: str) -> Token:
        self.skip_whitespace()
        if not self.curr or not self.curr.data or self.curr.data.type != t:
            got = type_to_string(self.curr.data.type) if (self.curr and self.curr.data) else "END"
            raise RuntimeError(f"{msg} (got {got})")
        tok = self.curr.data
        self.advance()
        return tok

    # ---------- precedence ----------
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

    # ---------- program ----------
    def parse_program(self) -> Program:
        body: List[Stmt] = []
        self.skip_whitespace()
        while self.curr and self.curr.data and self.curr.data.type != Type.END:
            body.append(self.parse_statement())
        return Program(body)

    # ---------- statements ----------
    def parse_statement(self) -> Stmt:
        self.skip_whitespace()
        if not self.curr or not self.curr.data:
            raise RuntimeError("Unexpected end of input in statement.")

        t = self.curr.data.type

        if t == Type.CUBRACKET_L:
            return self.parse_block()

        if t == Type.LET:
            return self.parse_vardecl()

        if t == Type.RET:
            return self.parse_return()

        if t == Type.IF:
            return self.parse_if()

        if t == Type.WHILE:
            return self.parse_while()

        if t == Type.DEF:
            return self.parse_funcdef()

        # fallback: assignment or expression statement
        # lookahead: IDENT '=' ...
        if t == Type.IDENT:
            # capture name but don't advance yet
            name_tok = self.peek()
            # look ahead one token
            nxt = self.curr.next
            while nxt and nxt.data and nxt.data.type in (Type.WHITESPACE, Type.RAW, Type.NLINE):
                nxt = nxt.next
            if nxt and nxt.data and nxt.data.type == Type.EQ:
                # assignment
                self.advance()  # consume IDENT
                self.expect(Type.EQ, "Expected '=' in assignment")
                value = self.parse_expression()
                return Assign(self.src[name_tok.pos:name_tok.pos+name_tok.length], value)

        # expression statement
        return ExprStmt(self.parse_expression())

    def parse_block(self) -> Block:
        self.expect(Type.CUBRACKET_L, "Expected '{' to start block")
        stmts: List[Stmt] = []
        self.skip_whitespace()
        while self.curr and self.curr.data and self.curr.data.type != Type.CUBRACKET_R:
            stmts.append(self.parse_statement())
            self.skip_whitespace()
        self.expect(Type.CUBRACKET_R, "Expected '}' to end block")
        return Block(stmts)

    def parse_vardecl(self) -> VarDecl:
        self.expect(Type.LET, "Expected 'let'")
        name_tok = self.expect(Type.IDENT, "Expected identifier after 'let'")
        init = None
        if self.match(Type.EQ):
            init = self.parse_expression()
        name = self.src[name_tok.pos:name_tok.pos + name_tok.length]
        return VarDecl(name, init)

    def parse_return(self) -> ReturnStmt:
        self.expect(Type.RET, "Expected 'ret'")
        # value optional (allow bare 'ret')
        self.skip_whitespace()
        if self.curr and self.curr.data and self.curr.data.type not in (Type.CUBRACKET_R, Type.END):
            # try parse expression; if next is '}' or END, treat as no value
            try:
                val = self.parse_expression()
                return ReturnStmt(val)
            except RuntimeError:
                return ReturnStmt(None)
        return ReturnStmt(None)

    def parse_if(self) -> IfStmt:
        self.expect(Type.IF, "Expected 'if'")
        cond = self.parse_expression()
        then_block = self.parse_block()

        elifs: List[tuple[Expr, Block]] = []
        else_block: Optional[Block] = None

        # zero or more elif
        while True:
            self.skip_whitespace()
            if self.curr and self.curr.data and self.curr.data.type == Type.ELIF:
                self.advance()
                ec = self.parse_expression()
                eb = self.parse_block()
                elifs.append((ec, eb))
            else:
                break

        # optional else ('el' in your token set)
        self.skip_whitespace()
        if self.curr and self.curr.data and self.curr.data.type == Type.EL:
            self.advance()
            else_block = self.parse_block()

        return IfStmt(cond, then_block, elifs, else_block)

    def parse_while(self) -> WhileStmt:
        self.expect(Type.WHILE, "Expected 'while'")
        cond = self.parse_expression()
        body = self.parse_block()
        return WhileStmt(cond, body)

    def parse_funcdef(self) -> FuncDef:
        self.expect(Type.DEF, "Expected 'def'")
        name_tok = self.expect(Type.IDENT, "Expected function name")
        name = self.src[name_tok.pos:name_tok.pos + name_tok.length]
        self.expect(Type.SMBRACKET_L, "Expected '(' after function name")
        params: List[str] = []
        # param list: IDENT (, IDENT)*
        self.skip_whitespace()
        if self.curr and self.curr.data and self.curr.data.type == Type.IDENT:
            while True:
                p_tok = self.expect(Type.IDENT, "Expected parameter name")
                params.append(self.src[p_tok.pos:p_tok.pos + p_tok.length])
                if not self.match(Type.COMMA):
                    break
        self.expect(Type.SMBRACKET_R, "Expected ')' after parameters")
        body = self.parse_block()
        return FuncDef(name, params, body)

    # ---------- expressions ----------
    def parse_expression(self) -> Expr:
        lhs = self.parse_primary()
        return self.parse_binary_op_rhs(0, lhs)

    def parse_primary(self) -> Expr:
        if not self.curr or not self.curr.data:
            raise RuntimeError("Unexpected end of input to primary expression.")
        tok = self.peek()

        if tok.type == Type.IDENT:
            name = self.src[tok.pos: tok.pos + tok.length]
            self.advance()
            # function call?
            if self.match(Type.SMBRACKET_L):
                args: List[Expr] = []
                self.skip_whitespace()
                if self.curr and self.curr.data and self.curr.data.type != Type.SMBRACKET_R:
                    while True:
                        args.append(self.parse_expression())
                        if not self.match(Type.COMMA):
                            break
                self.expect(Type.SMBRACKET_R, "Expected ')' after arguments")
                return CallExpr(name, args)
            return VariableExpr(name)

        if tok.type in (Type.DECIMAL, Type.NUMBER):
            value = float(self.src[tok.pos: tok.pos + tok.length])
            self.advance()
            return NumberExpr(value)

        if tok.type == Type.SMBRACKET_L:
            self.advance()
            expr = self.parse_expression()
            if not self.match(Type.SMBRACKET_R):
                raise RuntimeError("Expected closing ')'")
            return expr

        raise RuntimeError(f"Unknown token in expression: {type_to_string(tok.type)}")

    def parse_binary_op_rhs(self, min_prec: int, lhs: Expr) -> Expr:
        while True:
            self.skip_whitespace()
            op_type = self.curr.data.type if (self.curr and self.curr.data) else Type.END
            op_prec = self.get_precedence(op_type)
            if op_prec < min_prec:
                break

            tok = self.curr.data
            op_symbol = self.src[tok.pos: tok.pos + tok.length]
            self.curr = self.curr.next

            self.skip_whitespace()
            rhs = self.parse_primary()

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

            lhs = BinaryExpr(op_symbol, lhs, rhs)

        return lhs

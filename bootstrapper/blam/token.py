from enum import IntEnum
from dataclasses import dataclass
from typing import Dict
import re


# -----------------------------
# Token types
# -----------------------------
class Type(IntEnum):
    # ordered in parsing hierarchy
    SKIP = -2   # used in parser for skippable tokens
    END = 0     # end of file marker
    RAW = 1     # raw src

    # comments
    MLINE_COMMENT = 2  # #* *#
    SLINE_COMMENT = 3  # # \n

    # strings
    STRING = 4  # "example"

    # reserved keywords
    AND = 5
    OR = 6
    DEF = 7
    RET = 8
    IF = 9
    EL = 10
    ELIF = 11
    WHILE = 12
    FOR = 13
    IN = 14
    BREAK = 15
    CONTINUE = 16
    TRUE = 17
    FALSE = 18
    NIL = 19
    PUB = 20
    CONST = 21
    LET = 22

    # any text
    IDENT = 23

    # numbers
    DECIMAL = 24
    NUMBER = 25

    # space
    NLINE = 26
    WHITESPACE = 27

    # delimiters
    SMBRACKET_L = 28
    SMBRACKET_R = 29
    SQBRACKET_L = 30
    SQBRACKET_R = 31
    CUBRACKET_L = 32
    CUBRACKET_R = 33

    # operators
    ARROW = 34
    BAND = 35
    BOR = 36
    BXOR = 37
    BLS = 38
    BRS = 39
    DOT = 40
    COMMA = 41
    COLON = 42
    AT = 43

    # comparison
    EQEQ = 44
    NEQ = 45
    LT = 46
    GT = 47
    LTE = 48
    GTE = 49

    # arithmetic
    PLUSEQ = 50
    MINEQ = 51
    DIVEQ = 52
    MULTEQ = 53
    EQ = 54
    PLUS = 55
    MIN = 56
    DIV = 57
    EXP = 58
    MULT = 59


# -----------------------------
# Type to string
# -----------------------------
def type_to_string(t: Type) -> str:
    return t.name if isinstance(t, Type) else "UNKNOWN"


ORDER = [
    # 1) comments and strings
    Type.MLINE_COMMENT,
    Type.SLINE_COMMENT,
    Type.STRING,

    # 2) literals, identifiers, keywords
    Type.DECIMAL,
    Type.NUMBER,
    Type.AND,
    Type.OR,
    Type.DEF,
    Type.RET,
    Type.IF,
    Type.EL,
    Type.ELIF,
    Type.WHILE,
    Type.FOR,
    Type.IN,
    Type.BREAK,
    Type.CONTINUE,
    Type.TRUE,
    Type.FALSE,
    Type.NIL,
    Type.PUB,
    Type.CONST,
    Type.LET,
    Type.IDENT,

    # 3) multi-char operators (must come before their single-char parts)
    Type.EXP,      # ** before *
    Type.PLUSEQ,
    Type.MINEQ,
    Type.DIVEQ,
    Type.MULTEQ,
    Type.EQEQ,     # == before =
    Type.NEQ,      # != before !
    Type.LTE,      # <= before <
    Type.GTE,      # >= before >
    Type.BLS,      # << before <
    Type.BRS,      # >> before >
    Type.ARROW,    # -> before - and >

    # 4) single-char operators & punctuation
    Type.PLUS,
    Type.MIN,
    Type.MULT,
    Type.DIV,
    Type.EQ,
    Type.LT,
    Type.GT,
    Type.BAND,
    Type.BOR,
    Type.BXOR,
    Type.DOT,
    Type.COMMA,
    Type.COLON,
    Type.AT,

    # 5) delimiters
    Type.SMBRACKET_L,
    Type.SMBRACKET_R,
    Type.SQBRACKET_L,
    Type.SQBRACKET_R,
    Type.CUBRACKET_L,
    Type.CUBRACKET_R,

    # 6) spacing last
    Type.NLINE,
    Type.WHITESPACE,
]

TOKEN_EXPR: Dict[Type, str] = {
    Type.MLINE_COMMENT: r"#\*[\s\S]*?\*#",
    Type.SLINE_COMMENT: r"#([^\n]*)(\n|$)",
    Type.STRING: r"\"(\\.|[^\"\\])*\"|'(\\.|[^'\\])*'",
    Type.AND: r"\band\b",
    Type.OR: r"\bor\b",
    Type.DEF: r"\bdef\b",
    Type.RET: r"\bret\b",
    Type.IF: r"\bif\b",
    Type.EL: r"\bel\b",
    Type.ELIF: r"\belif\b",
    Type.WHILE: r"\bwhile\b",
    Type.FOR: r"\bfor\b",
    Type.IN: r"\bin\b",
    Type.BREAK: r"\bbreak\b",
    Type.CONTINUE: r"\bcontinue\b",
    Type.TRUE: r"\btrue\b",
    Type.FALSE: r"\bfalse\b",
    Type.NIL: r"\bnil\b",
    Type.PUB: r"\bpub\b",
    Type.CONST: r"\bconst\b",
    Type.LET: r"\blet\b",
    Type.IDENT: r"[a-zA-Z_][a-zA-Z0-9_]*",
    Type.DECIMAL: r"(?:\d+\.\d*|\.\d+)",
    Type.NUMBER: r"\d+",
    Type.NLINE: r"\n",
    Type.WHITESPACE: r"\s+",
    Type.SMBRACKET_L: r"\(",
    Type.SMBRACKET_R: r"\)",
    Type.SQBRACKET_L: r"\[",
    Type.SQBRACKET_R: r"\]",
    Type.CUBRACKET_L: r"\{",
    Type.CUBRACKET_R: r"\}",
    Type.ARROW: r"->",
    Type.BAND: r"&",
    Type.BOR: r"\|",
    Type.BXOR: r"\^",
    Type.BLS: r"<<",
    Type.BRS: r">>",
    Type.DOT: r"\.",
    Type.COMMA: r",",
    Type.COLON: r":",
    Type.AT: r"@",
    Type.EQEQ: r"==",
    Type.NEQ: r"!=",
    Type.LTE: r"<=",
    Type.GTE: r">=",
    Type.LT: r"<",
    Type.GT: r">",
    Type.PLUSEQ: r"\+=",
    Type.MINEQ: r"-=",
    Type.DIVEQ: r"/=",
    Type.MULTEQ: r"\*=",
    Type.EQ: r"=",
    Type.PLUS: r"\+",
    Type.MIN: r"-",
    Type.DIV: r"/",
    Type.EXP: r"\*\*",
    Type.MULT: r"\*"
}


# -----------------------------
# Token class
# -----------------------------
@dataclass
class Token:
    type: Type
    pos: int
    length: int

    def to_string(self, src: str) -> str:
        content = src[self.pos:self.pos + self.length]
        return (f"Type: {type_to_string(self.type)}, "
                f"Pos: {self.pos}, Len: {self.length}, "
                f"Content:\n{content}\n")
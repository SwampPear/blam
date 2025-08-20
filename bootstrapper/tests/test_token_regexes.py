import re
import pytest
from blam.tokenizer import TOKEN_EXPR, Type


def matches(ttype: Type, text: str) -> bool:
    """Return True if regex for ttype matches the whole text."""
    pattern = TOKEN_EXPR[ttype]
    return re.fullmatch(pattern, text) is not None


@pytest.mark.parametrize("src", ["#* multiline *#", "#* with\nnewlines *#"])
def test_mline_comment(src):
    assert matches(Type.MLINE_COMMENT, src)


@pytest.mark.parametrize("src", ["# this is a comment\n", "# comment at end"])
def test_sline_comment(src):
    assert matches(Type.SLINE_COMMENT, src)


@pytest.mark.parametrize("src", ['"hello"', '"escaped \\" quote"'])
def test_string(src):
    assert matches(Type.STRING, src)


@pytest.mark.parametrize("word,ttype", [
    ("and", Type.AND),
    ("or", Type.OR),
    ("def", Type.DEF),
    ("ret", Type.RET),
    ("if", Type.IF),
    ("el", Type.EL),
    ("elif", Type.ELIF),
    ("while", Type.WHILE),
    ("for", Type.FOR),
    ("in", Type.IN),
    ("break", Type.BREAK),
    ("continue", Type.CONTINUE),
    ("true", Type.TRUE),
    ("false", Type.FALSE),
    ("nil", Type.NIL),
    ("pub", Type.PUB),
    ("const", Type.CONST),
    ("let", Type.LET),
])
def test_keywords(word, ttype):
    assert matches(ttype, word)


@pytest.mark.parametrize("ident", ["x", "foo123", "_var"])
def test_ident(ident):
    assert matches(Type.IDENT, ident)


@pytest.mark.parametrize("num", ["123", "0", "42"])
def test_number(num):
    assert matches(Type.NUMBER, num)


@pytest.mark.parametrize("num", ["3.14", "0.5", ".25", "12."])
def test_decimal(num):
    assert matches(Type.DECIMAL, num)


def test_newline():
    assert matches(Type.NLINE, "\n")


@pytest.mark.parametrize("ws", [" ", "   ", "\t", " \t\n"])
def test_whitespace(ws):
    assert matches(Type.WHITESPACE, ws)


@pytest.mark.parametrize("src,ttype", [
    ("(", Type.SMBRACKET_L),
    (")", Type.SMBRACKET_R),
    ("[", Type.SQBRACKET_L),
    ("]", Type.SQBRACKET_R),
    ("{", Type.CUBRACKET_L),
    ("}", Type.CUBRACKET_R),
    ("->", Type.ARROW),
    ("&", Type.BAND),
    ("|", Type.BOR),
    ("^", Type.BXOR),
    ("<<", Type.BLS),
    (">>", Type.BRS),
    (".", Type.DOT),
    (",", Type.COMMA),
    (":", Type.COLON),
    ("@", Type.AT),
    ("==", Type.EQEQ),
    ("!=", Type.NEQ),
    ("<=", Type.LTE),
    (">=", Type.GTE),
    ("<", Type.LT),
    (">", Type.GT),
    ("+=", Type.PLUSEQ),
    ("-=", Type.MINEQ),
    ("/=", Type.DIVEQ),
    ("*=", Type.MULTEQ),
    ("=", Type.EQ),
    ("+", Type.PLUS),
    ("-", Type.MIN),
    ("/", Type.DIV),
    ("**", Type.EXP),
    ("*", Type.MULT),
])

def test_symbols_and_ops(src, ttype):
    assert matches(ttype, src)

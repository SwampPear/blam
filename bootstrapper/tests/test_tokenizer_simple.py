import pytest
from blam.tokenizer import tokenize, Type


def collect_tokens(lst, src):
    result = []
    node = lst.head
    while node:
        t = node.data.type
        text = src[node.data.pos: node.data.pos + node.data.length]
        result.append((t, text))
        node = node.next
    return result


def expect_sequence(lst, src, expected_types):
    result = collect_tokens(lst, src)
    actual_types = [t for (t, _) in result if t not in (Type.RAW, Type.WHITESPACE)]
    assert actual_types == expected_types


def test_single_identifier():
    src = "hello"
    tokens = tokenize(src)
    expect_sequence(tokens, src, [Type.IDENT])


def test_multiple_identifiers():
    src = "hello this is a test"
    tokens = tokenize(src)
    expect_sequence(tokens, src, [Type.IDENT] * 5)


def test_keywords():
    src = "def if el elif while for in ret break continue"
    tokens = tokenize(src)
    expect_sequence(tokens, src, [
        Type.DEF, Type.IF, Type.EL, Type.ELIF, Type.WHILE,
        Type.FOR, Type.IN, Type.RET, Type.BREAK, Type.CONTINUE
    ])


def test_numbers_and_decimals():
    src = "42 3.14"
    tokens = tokenize(src)
    expect_sequence(tokens, src, [Type.NUMBER, Type.DECIMAL])


def test_operators():
    src = "+ - * / ** += -= /= *= == != < > <= >="
    tokens = tokenize(src)
    types = [t for (t, _) in collect_tokens(tokens, src)]
    expected = {
        Type.PLUS, Type.MIN, Type.MULT, Type.DIV, Type.EXP,
        Type.PLUSEQ, Type.MINEQ, Type.DIVEQ, Type.MULTEQ,
        Type.EQEQ, Type.NEQ, Type.LT, Type.GT, Type.LTE, Type.GTE
    }
    assert expected.issubset(set(types))


def test_delimiters():
    src = "()[]{}"
    tokens = tokenize(src)
    types = [t for (t, _) in collect_tokens(tokens, src)]
    assert types[:6] == [
        Type.SMBRACKET_L, Type.SMBRACKET_R,
        Type.SQBRACKET_L, Type.SQBRACKET_R,
        Type.CUBRACKET_L, Type.CUBRACKET_R,
    ]


def test_string_literal():
    src = '"hello"'
    tokens = tokenize(src)
    collected = collect_tokens(tokens, src)
    assert len(collected) == 1
    assert collected[0][0] == Type.STRING


def test_comments():
    src = "# this is a comment\n#* multiline \ncomment *#"
    tokens = tokenize(src)
    types = [t for (t, _) in collect_tokens(tokens, src)]
    assert Type.SLINE_COMMENT in types
    assert Type.MLINE_COMMENT in types


def test_whitespace_and_newline():
    src = "let x = 5\n"
    tokens = tokenize(src)
    types = [t for (t, _) in collect_tokens(tokens, src)]
    assert Type.LET in types
    assert Type.NLINE in types


def test_exponentiation_operator():
    src = "2 ** 3"
    tokens = tokenize(src)
    expect_sequence(tokens, src, [Type.NUMBER, Type.EXP, Type.NUMBER])
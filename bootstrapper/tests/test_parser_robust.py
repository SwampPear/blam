import pytest
from blam.tokenizer import tokenize, Type
from blam.parser import Parser
from blam.ast import NumberExpr, VariableExpr, BinaryExpr


def parse_one(src: str):
    toks = tokenize(src)
    p = Parser(toks.head, src)
    ast = p.parse()
    assert len(ast) == 1, f"expected 1 expr, got {len(ast)}"
    return ast[0]


def is_op(node, op):
    return isinstance(node, BinaryExpr) and node.op == op


def as_bin(node):
    assert isinstance(node, BinaryExpr)
    return node


def as_num(node, val=None):
    assert isinstance(node, NumberExpr)
    if val is not None:
        assert node.value == pytest.approx(val)
    return node


def as_var(node, name=None):
    assert isinstance(node, VariableExpr)
    if name is not None:
        assert node.name == name
    return node


# ---------- Precedence & associativity ----------

def test_prec_add_before_relational_and_eq():
    # a + 1 < b == c + 2  ->  ((a + 1) < b) == (c + 2)
    e = parse_one("a + 1 < b == c + 2")
    assert is_op(e, "==")
    left = as_bin(e.lhs)
    right = as_bin(e.rhs)
    assert is_op(left, "<")
    assert is_op(right, "+")
    assert is_op(left.lhs, "+")
    as_var(left.rhs, "b")
    as_var(right.lhs, "c")
    as_num(right.rhs, 2)


def test_prec_mul_over_add_and_rel():
    # a + b * 2 < c + d * 3
    e = parse_one("a + b * 2 < c + d * 3")
    assert is_op(e, "<")
    l = as_bin(e.lhs)   # a + (b * 2)
    r = as_bin(e.rhs)   # c + (d * 3)
    assert is_op(l, "+") and is_op(l.rhs, "*")
    assert is_op(r, "+") and is_op(r.rhs, "*")


def test_exp_right_associative_chain():
    # 2 ** 3 ** 2 => 2 ** (3 ** 2)
    e = parse_one("2 ** 3 ** 2")
    assert is_op(e, "**")
    assert is_op(e.rhs, "**")
    as_num(e.lhs, 2)
    as_num(e.rhs.lhs, 3)
    as_num(e.rhs.rhs, 2)


def test_left_associativity_for_add_mul_div_sub():
    # a - b - c -> (a - b) - c
    e = parse_one("a - b - c")
    assert is_op(e, "-")
    assert is_op(e.lhs, "-")

    # a / b * c -> (a / b) * c
    e2 = parse_one("a / b * c")
    assert is_op(e2, "*")
    assert is_op(e2.lhs, "/")


def test_lte_gte_vs_lt_gt_eq():
    e = parse_one("a <= b >= c < d > e == f")
    # ((((a <= b) >= c) < d) > e) == f
    assert is_op(e, "==")
    chain = e.lhs
    for op in [">", "<", ">=", "<="][::-1]:  # unwind expected structure
        assert isinstance(chain, BinaryExpr)
        chain = chain.lhs
    # just smoke-check the top-level sequence present:
    s = " ".join(
        [op.op for op in [as_bin(e.lhs), as_bin(as_bin(e.lhs).lhs),
                          as_bin(as_bin(as_bin(e.lhs).lhs).lhs),
                          as_bin(as_bin(as_bin(as_bin(e.lhs).lhs).lhs).lhs)]]
    )
    assert set(s.split()) == {"<=", ">=", "<", ">"}


# ---------- Parentheses & grouping ----------

def test_parentheses_override_everything():
    e = parse_one("(a + b) * (c + d)")
    assert is_op(e, "*")
    assert is_op(e.lhs, "+")
    assert is_op(e.rhs, "+")


def test_nested_parens_deep():
    e = parse_one("(((1 + 2))) * (3 + (4 * (5)))")
    assert is_op(e, "*")
    as_num(as_bin(e.lhs).lhs, 1)
    as_num(as_bin(e.lhs).rhs, 2)
    r = as_bin(e.rhs)
    assert is_op(r, "+")
    assert is_op(r.rhs, "*")


def test_missing_rparen_raises():
    with pytest.raises(RuntimeError, match=r"Expected closing '\)'"):
        parse_one("(a + 1")


# ---------- Primaries & literals ----------

def test_number_and_decimal_parsing():
    as_num(parse_one("42"), 42.0)
    as_num(parse_one("3.14"), 3.14)
    as_num(parse_one(".5"), 0.5)
    as_num(parse_one("12."), 12.0)


def test_variable_primary_and_spacing():
    as_var(parse_one("   x  "), "x")
    as_var(parse_one("\n\nx"), "x")


# ---------- Whitespace / newlines handling ----------

def test_whitespace_newlines_between_tokens():
    e = parse_one("a \n + \t b \n * 2")
    assert is_op(e, "+")
    assert is_op(e.rhs, "*")


# ---------- Error cases ----------

def test_unknown_string_token_is_error():
    with pytest.raises(RuntimeError, match=r"Unknown token in expression: STRING"):
        parse_one('"hello"')


def test_unexpected_end_of_stream():
    # Tokenizer produces RAW if unmatched, but ensure parser trips when it runs out inside parens
    with pytest.raises(RuntimeError, match=r"Unexpected end of input to primary expression"):
        parse_one("(")


def test_comment_tokens_cause_error_if_present():
    # parser does not skip comments; tokenizer keeps them => should be error
    with pytest.raises(RuntimeError):
        parse_one("a # comment\n + b")


# ---------- Bigger integration-like scenarios ----------

def test_program_like_snippet_expression():
    # Just one expression test—parser currently parses one expr at a time
    e = parse_one("a + b * c ** 2 - d / (e + f)")
    # top '-'
    assert is_op(e, "-")
    left = as_bin(e.lhs)             # a + (b * (c ** 2))
    right = as_bin(e.rhs)            # d / (e + f)
    assert is_op(left, "+")
    assert is_op(left.rhs, "*")
    assert is_op(as_bin(left.rhs).rhs, "**")
    assert is_op(right, "/")
    assert is_op(right.rhs, "+")


def test_chain_relational_then_equality_grouping():
    # (a < b) == (c >= d)
    e = parse_one("a < b == c >= d")
    assert is_op(e, "==")
    assert is_op(e.lhs, "<")
    assert is_op(e.rhs, ">=")


def test_mix_of_spaces_and_parens_everywhere():
    e = parse_one(" ( 1+ 2 )**( 3  +4 ) ")
    assert is_op(e, "**")
    assert is_op(e.lhs, "+")
    assert is_op(e.rhs, "+")
    as_num(as_bin(e.lhs).lhs, 1)
    as_num(as_bin(e.lhs).rhs, 2)
    as_num(as_bin(e.rhs).lhs, 3)
    as_num(as_bin(e.rhs).rhs, 4)

import pytest
from blam.tokenizer import tokenize
from blam.parser import Parser
from blam.ast import NumberExpr, VariableExpr, BinaryExpr, ExprStmt, Program


def parse_expr(src: str):
    """Parse a single *expression* using the new program parser, return the inner Expr."""
    toks = tokenize(src)
    p = Parser(toks.head, src)
    prog = p.parse_program()
    assert isinstance(prog, Program)
    assert len(prog.body) == 1, f"expected 1 top-level stmt, got {len(prog.body)}"
    stmt = prog.body[0]
    assert isinstance(stmt, ExprStmt), f"expected an expression statement, got {type(stmt).__name__}"
    return stmt.expr


def op_of(node):
    assert isinstance(node, BinaryExpr)
    return node.op


def test_variable_and_number_primaries():
    assert isinstance(parse_expr("x"), VariableExpr)
    e = parse_expr("42")
    assert isinstance(e, NumberExpr)
    assert e.value == 42.0


def test_parenthesized_expression():
    e = parse_expr("(1)")
    assert isinstance(e, NumberExpr) and e.value == 1.0


def test_missing_closing_paren_raises():
    with pytest.raises(RuntimeError, match=r"Expected closing '\)'"):
        parse_expr("(1 + 2")


def test_unknown_token_in_expression_string_is_error():
    with pytest.raises(RuntimeError, match=r"Unknown token in expression: STRING"):
        parse_expr('"hello"')


def test_add_then_mul_precedence():
    # a + b * 2  =>  a + (b * 2)
    e = parse_expr("a + b * 2")
    assert isinstance(e, BinaryExpr) and op_of(e) == "+"
    assert isinstance(e.rhs, BinaryExpr) and op_of(e.rhs) == "*"


def test_parens_override_precedence():
    # (a + b) * 2
    e = parse_expr("(a + b) * 2")
    assert isinstance(e, BinaryExpr) and op_of(e) == "*"
    assert isinstance(e.lhs, BinaryExpr) and op_of(e.lhs) == "+"


def test_left_associativity_plus_minus():
    # a - b - c  =>  (a - b) - c
    e = parse_expr("a - b - c")
    assert isinstance(e, BinaryExpr) and op_of(e) == "-"
    left = e.lhs
    assert isinstance(left, BinaryExpr) and op_of(left) == "-"


def test_right_associativity_exponent():
    # 2 ** 3 ** 2  =>  2 ** (3 ** 2)
    e = parse_expr("2 ** 3 ** 2")
    assert isinstance(e, BinaryExpr) and op_of(e) == "**"
    # rhs must be another '**'
    assert isinstance(e.rhs, BinaryExpr) and op_of(e.rhs) == "**"


def test_div_and_mul_same_prec_left_assoc():
    # a / b * c  =>  (a / b) * c
    e = parse_expr("a / b * c")
    assert isinstance(e, BinaryExpr) and op_of(e) == "*"
    assert isinstance(e.lhs, BinaryExpr) and op_of(e.lhs) == "/"


def test_comparisons_prec_lower_than_add_mul():
    # a + 1 < b * 2  =>  (a + 1) < (b * 2)
    e = parse_expr("a + 1 < b * 2")
    assert isinstance(e, BinaryExpr) and op_of(e) == "<"
    assert isinstance(e.lhs, BinaryExpr) and op_of(e.lhs) == "+"
    assert isinstance(e.rhs, BinaryExpr) and op_of(e.rhs) == "*"


def test_equality_prec_lower_than_relational():
    # a < b == c < d  =>  (a < b) == (c < d)
    e = parse_expr("a < b == c < d")
    assert isinstance(e, BinaryExpr) and op_of(e) == "=="
    assert isinstance(e.lhs, BinaryExpr) and op_of(e.lhs) == "<"
    assert isinstance(e.rhs, BinaryExpr) and op_of(e.rhs) == "<"


def test_whitespace_and_newlines_are_ignored_between_ops():
    e = parse_expr("a \n  +   \n b")
    assert isinstance(e, BinaryExpr) and op_of(e) == "+"


def test_nested_expression_chain():
    # a + b * c ** 2 - d / (e + f)
    e = parse_expr("a + b * c ** 2 - d / (e + f)")
    # top is '-'
    assert isinstance(e, BinaryExpr) and op_of(e) == "-"
    # left side is (a + (b * (c ** 2)))
    left = e.lhs
    assert isinstance(left, BinaryExpr) and op_of(left) == "+"
    mul = left.rhs
    assert isinstance(mul, BinaryExpr) and op_of(mul) == "*"
    exp = mul.rhs
    assert isinstance(exp, BinaryExpr) and op_of(exp) == "**"
    # right side is d / (e + f)
    right = e.rhs
    assert isinstance(right, BinaryExpr) and op_of(right) == "/"
    assert isinstance(right.rhs, BinaryExpr) and op_of(right.rhs) == "+"

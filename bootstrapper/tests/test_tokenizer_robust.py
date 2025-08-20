import pytest
from blam.tokenizer import tokenize, Type

def flatten_types(src):
    lst = tokenize(src)
    out = []
    n = lst.head
    while n:
        out.append(n.data.type)
        n = n.next
    return out

def flatten_lexemes(src):
    lst = tokenize(src)
    out = []
    n = lst.head
    while n:
        s = src[n.data.pos:n.data.pos + n.data.length]
        out.append((n.data.type, s))
        n = n.next
    return out

def test_no_whitespace_tokens():
    types = flatten_types("a  b\tc\n")
    assert Type.WHITESPACE not in types
    assert Type.NLINE in types

def test_lte_and_gte_vs_lt_gt_eq():
    types = flatten_types("<= >= < > =")
    # Expect LTE, GTE, LT, GT, EQ — in that order
    assert types[:5] == [Type.LTE, Type.GTE, Type.LT, Type.GT, Type.EQ]

def test_eqeq_vs_eq_and_neq():
    types = flatten_types("== = !=")
    assert types[:3] == [Type.EQEQ, Type.EQ, Type.NEQ]

def test_exp_vs_mult():
    types = flatten_types("** * **")
    # EXP, MULT, EXP
    assert [t for t in types[:3]] == [Type.EXP, Type.MULT, Type.EXP]

def test_shift_vs_gt_lt():
    types = flatten_types("<< >> < >")
    assert types[:4] == [Type.BLS, Type.BRS, Type.LT, Type.GT]

def test_arrow_before_minus_and_gt():
    types = flatten_types("-> -> - >")
    assert types[:4] == [Type.ARROW, Type.ARROW, Type.MIN, Type.GT]

def test_many_of_same_operator():
    types = flatten_types("++++--**//")
    # PLUS PLUS PLUS PLUS MIN MIN EXP DIV DIV (order depends only on reading)
    expected = [Type.PLUS, Type.PLUS, Type.PLUS, Type.PLUS,
                Type.MIN, Type.MIN, Type.EXP, Type.DIV, Type.DIV]
    assert types[:len(expected)] == expected

def test_keywords_have_word_boundaries():
    # 'anderson' should be IDENT, not AND; same for 'elifx'
    pairs = flatten_lexemes("and anderson elif elifx in input")
    assert pairs[0] == (Type.AND, "and")
    assert pairs[1][0] == Type.IDENT and pairs[1][1] == "anderson"
    assert pairs[2] == (Type.ELIF, "elif")
    assert pairs[3][0] == Type.IDENT and pairs[3][1] == "elifx"
    # 'in' vs 'input'
    assert pairs[4] == (Type.IN, "in")
    assert pairs[5][0] == Type.IDENT and pairs[5][1] == "input"

def test_ident_forms():
    lex = flatten_lexemes("_x X_1 foo_bar _ __ _a1")
    for t, s in lex:
        assert t in {Type.IDENT, Type.NLINE}  # RAW only for spaces if any
    idents = [s for (t, s) in lex if t == Type.IDENT]
    assert idents == ["_x", "X_1", "foo_bar", "_", "__", "_a1"]

def test_numbers_and_decimals_edges():
    pairs = flatten_lexemes("0 42 12. .25 3. .0")
    # Note: DECIMAL allows trailing dot per pattern \d+\.\d*
    expected = [Type.NUMBER, Type.NUMBER, Type.DECIMAL, Type.DECIMAL, Type.DECIMAL, Type.DECIMAL]
    assert [t for (t, _) in pairs if t != Type.NLINE] == expected

def test_adjacent_tokens_without_spaces():
    pairs = flatten_lexemes("a+=b==c<=d**e")
    expected_types = [Type.IDENT, Type.PLUSEQ, Type.IDENT, Type.EQEQ, Type.IDENT,
                      Type.LTE, Type.IDENT, Type.EXP, Type.IDENT]
    assert [t for (t, _) in pairs] == expected_types

def test_brackets_and_punctuation():
    pairs = flatten_lexemes("([x],{y})")
    expected = [Type.SMBRACKET_L, Type.SQBRACKET_L, Type.IDENT, Type.SQBRACKET_R,
                Type.COMMA, Type.CUBRACKET_L, Type.IDENT, Type.CUBRACKET_R, Type.SMBRACKET_R]
    assert [t for (t, _) in pairs] == expected

def test_string_does_not_tokenize_inside():
    pairs = flatten_lexemes('"a <= b ** c == d"')
    # Entire thing should be a single STRING
    assert len(pairs) == 1 and pairs[0][0] == Type.STRING

def test_string_escapes_and_quotes():
    pairs = flatten_lexemes(r'"he said \"hi\"" \'it\\\'s ok\'')
    types = [t for (t, _) in pairs if t != Type.NLINE]

    # First token should be STRING, second should fall back to IDENT/RAW since single quotes aren't strings anymore
    assert types[0] == Type.STRING
    assert Type.STRING not in types[1:]


def test_single_line_comment_eats_rest_of_line():
    pairs = flatten_lexemes("x # comment <= ** ==\ny")
    # Expect IDENT, SLINE_COMMENT, NLINE, IDENT (no operators from the comment)
    types = [t for (t, _) in pairs]
    assert types == [Type.IDENT, Type.SLINE_COMMENT, Type.IDENT]

def test_multi_line_comment_shields_interior():
    src = "a #* x <= y ** z == w *# b"
    pairs = flatten_lexemes(src)
    # IDENT, MLINE_COMMENT, IDENT
    assert [t for (t, _) in pairs] == [Type.IDENT, Type.MLINE_COMMENT, Type.IDENT]

def test_dot_and_ident_separation():
    pairs = flatten_lexemes("obj.method . standalone")
    # IDENT DOT IDENT DOT IDENT
    filtered = [t for (t, _) in pairs if t != Type.NLINE]
    assert filtered == [Type.IDENT, Type.DOT, Type.IDENT, Type.DOT, Type.IDENT]

def test_comma_colon_at_tokens():
    pairs = flatten_lexemes("@x,y:z")
    assert [t for (t, _) in pairs] == [Type.AT, Type.IDENT, Type.COMMA, Type.IDENT, Type.COLON, Type.IDENT]

def test_mixed_program_snippet():
    src = '''
pub def f(x, y) {
  let a = 2**x + y/3.14
  if a >= 10 {
    ret a
  } el {
    ret a-1
  }
}
'''
    types = [t for (t, _) in flatten_lexemes(src)]
    # Smoke check: ensure some key tokens are present and in order
    must_appear_in_order = [
        Type.PUB, Type.DEF, Type.IDENT, Type.SMBRACKET_L, Type.IDENT, Type.COMMA, Type.IDENT,
        Type.SMBRACKET_R, Type.CUBRACKET_L, Type.LET, Type.IDENT, Type.EQ, Type.NUMBER,
        Type.EXP, Type.IDENT, Type.PLUS, Type.IDENT, Type.DIV, Type.DECIMAL,
        Type.IF, Type.IDENT, Type.GTE, Type.NUMBER, Type.CUBRACKET_L, Type.RET, Type.IDENT,
        Type.CUBRACKET_R, Type.EL, Type.CUBRACKET_L, Type.RET, Type.IDENT, Type.MIN, Type.NUMBER,
        Type.CUBRACKET_R, Type.CUBRACKET_R
    ]
    it = iter(types)
    for t in must_appear_in_order:
        for u in it:
            if u == t:
                break
        else:
            assert False, f"Token {t} not found in order"

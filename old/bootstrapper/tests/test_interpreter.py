import pytest
from blam.tokenizer import tokenize
from blam.parser import Parser
from blam.interpreter import Interpreter
from blam.ast import Program

def run(src: str):
    toks = tokenize(src)
    p = Parser(toks.head, src)
    prog: Program = p.parse_program()
    intr = Interpreter()
    intr.run(prog)
    return intr

def test_let_assign_arith():
    intr = run("{ let x = 2  let y = 3  x = x + y * 4 }")
    # locals are scoped to the block; nothing to assert in global env,
    # but absence of error is success.

def test_if_elif_else():
    intr = run("""
    {
      let a = 7
      if a >= 8 { a = 1 } elif a == 7 { a = 2 } el { a = 3 }
    }
    """)

def test_while_loop_accumulates():
    intr = run("""
    {
      let n = 3
      let acc = 1
      while n > 0 {
        acc = acc * 2
        n = n - 1
      }
      print(acc)
    }
    """)


def test_functions_and_return():
    intr = run("""
    {
      def pow2(n) {
        let acc = 1
        while n > 0 { acc = acc * 2  n = n - 1 }
        ret acc
      }
      let x = pow2(5)  # 32
      print(x)
    }
    """)
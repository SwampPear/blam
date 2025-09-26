// src/parser/parser.ts
import * as AST from '../ast'
import { Token, Tokenizer, TokKind } from '../lexer/tokenizer'

type TT = TokKind
type Range = AST.Range
const isNL = (t?: Token) => t?.kind === TT.NL

export class Parser {
  private toks: Token[]
  private i = 0

  constructor(src: string) {
    this.toks = new Tokenizer(src).tokenize()
  }

  parseProgram(): AST.Program {
    const start = this.pos()
    const decls: AST.TopLevel[] = []
    this.skipNL()
    while (!this.is(TT.EOF)) {
      decls.push(this.parseTop())
      this.skipNL()
    }
    const end = this.pos()
    this.semanticMainChecks(decls)
    return { kind: 'Program', decls, range: { start, end } }
  }

  // ——— top-level ———

  private parseTop(): AST.TopLevel {
    // import / const / struct / method / function
    if (this.is(TT.KwImport)) return this.parseImport()
    if (this.is(TT.KwConst)) return this.parseConstDeclNL()
    if (this.is(TT.KwStruct)) return this.parseStruct()
    // method: Identifier '::' Identifier '('
    if (this.is(TT.Identifier) && this.peekSym('::')) return this.parseMethod()
    return this.parseFuncDecl()
  }

  private parseImport(): AST.ImportDecl {
    const t0 = this.take(TT.KwImport)
    const name = this.expectIdent()
    let alias: AST.Identifier | undefined
    if (this.match(TT.KwAs)) alias = this.expectIdent()
    this.expectNL()
    return { kind: 'ImportDecl', name, alias, range: rng(t0.range.start, this.prevEnd()) }
  }

  private parseConstDeclNL(): AST.ConstDecl {
    const d = this.parseConstDecl()
    this.expectNL()
    return d
  }

  private parseConstDecl(): AST.ConstDecl {
    const t0 = this.take(TT.KwConst)
    const id = this.expectIdent()
    this.take(TT.Eq)
    const init = this.parseExpr()
    return { kind: 'ConstDecl', id, init, range: rng(t0.range.start, this.prevEnd()) }
  }

  private parseFuncDecl(): AST.FuncDecl {
    const sig = this.parseFuncSig()
    const body = this.parseBlock()
    return { kind: 'FuncDecl', sig, body, range: rng(sig.range.start, body.range.end) }
  }

  private parseFuncSig(): AST.FuncSig {
    const name = this.expectIdent()
    this.take(TT.LParen)
    const params = this.parseParamListOpt()
    this.take(TT.RParen)
    let ret: AST.TypeRef | undefined
    if (this.match(TT.Arrow)) ret = this.parseType()
    return { name, params, ret, range: rng(name.range.start, this.prevEnd()) }
  }

  private parseParamListOpt(): AST.FuncParam[] {
    const params: AST.FuncParam[] = []
    if (this.is(TT.RParen)) return params
    while (true) {
      // either "Type ident" or "ident"
      if (this.isTypeStart() && this.isAheadIdentAfterType()) {
        const ty = this.parseType()
        const name = this.expectIdent()
        params.push({ name, type: ty })
      } else {
        const name = this.expectIdent()
        // optional: allow leading type omitted
        params.push({ name })
      }
      if (!this.match(TT.Comma)) break
    }
    return params
  }

  private parseMethod(): AST.MethodDef {
    const recv = this.expectIdent()
    this.expectSym('::')
    const name = this.expectIdent()
    this.take(TT.LParen)
    const params = this.parseParamListOpt()
    this.take(TT.RParen)
    let ret: AST.TypeRef | undefined
    if (this.match(TT.Arrow)) ret = this.parseType()
    const body = this.parseBlock()
    return { kind: 'MethodDef', recv, name, params, ret, body, range: rng(recv.range.start, body.range.end) }
  }

  private parseStruct(): AST.StructDecl {
    const t0 = this.take(TT.KwStruct)
    const name = this.expectIdent()
    this.take(TT.LBrace)
    const members: AST.StructMember[] = []
    while (!this.is(TT.RBrace)) {
      this.skipNL()
      if (this.is(TT.RBrace)) break
      let pub = false
      if (this.match(TT.KwPub)) pub = true
      if (this.isTypeStart()) {
        const field = this.parseStructField(pub)
        members.push({ kind: 'StructField', field })
      } else {
        const sig = this.parseFuncSig()
        members.push({ kind: 'StructSig', pub, sig })
      }
      this.expectNL()
    }
    const rb = this.take(TT.RBrace)
    return { kind: 'StructDecl', name, members, range: rng(t0.range.start, rb.range.end) }
  }

  private parseStructField(pub: boolean): AST.StructField {
    const ty = this.parseType()
    const name = this.expectIdent()
    return { pub, name, type: ty, range: rng(ty.range.start, name.range.end) }
  }

  // ——— blocks & statements ———

  private parseBlock(): AST.Block {
    const lb = this.take(TT.LBrace)
    const stmts: AST.Stmt[] = []
    while (!this.is(TT.RBrace)) {
      this.skipNL()
      if (this.is(TT.RBrace)) break
      stmts.push(this.parseStmt())
      this.skipNL()
    }
    const rb = this.take(TT.RBrace)
    return { kind: 'Block', stmts, range: rng(lb.range.start, rb.range.end) }
  }

  private parseStmt(): AST.Stmt {
    if (this.is(TT.KwConst)) return this.parseConstDeclNL()
    if (this.isVarDeclStarter()) return this.parseVarDeclNL()
    if (this.isAssignStarter()) return this.parseAssignNL()
    if (this.is(TT.KwIf)) return this.parseIf()
    if (this.is(TT.KwSw)) return this.parseSwitch()
    if (this.is(TT.KwWhile)) return this.parseWhile()
    if (this.is(TT.KwDo)) return this.parseDoWhile()
    if (this.is(TT.KwFor)) return this.parseFor()
    if (this.is(TT.KwTry)) return this.parseTry()
    if (this.is(TT.KwReturn)) return this.parseReturnNL()
    if (this.is(TT.KwBreak)) { const t = this.take(TT.KwBreak); this.expectNL(); return { kind: 'BreakStmt', range: t.range } }
    if (this.is(TT.KwContinue)) { const t = this.take(TT.KwContinue); this.expectNL(); return { kind: 'ContinueStmt', range: t.range } }
    // expression statement
    const expr = this.parseExpr()
    this.expectNL()
    return { kind: 'ExprStmt', expr, range: rng(expr.range.start, this.prevEnd()) }
  }

  private parseVarDeclNL(): AST.VarDecl {
    const d = this.parseVarDecl()
    this.expectNL()
    return d
  }

  private parseVarDecl(): AST.VarDecl {
    // forms: Type ident '=' expr   |  ident '=' expr
    if (this.isTypeStart() && this.isAheadIdentAfterType()) {
      const ty = this.parseType()
      const name = this.expectIdent()
      this.take(TT.Eq)
      const init = this.parseExpr()
      return { kind: 'VarDecl', name, type: ty, init, range: rng(ty.range.start, init.range.end) }
    } else {
      const name = this.expectIdent()
      this.take(TT.Eq)
      const init = this.parseExpr()
      return { kind: 'VarDecl', name, init, range: rng(name.range.start, init.range.end) }
    }
  }

  private parseAssignNL(): AST.Assign {
    const a = this.parseAssign()
    this.expectNL()
    return a
  }
  private parseAssign(): AST.Assign {
    const target = this.parseLValue()
    this.take(TT.Eq)
    const value = this.parseExpr()
    return { kind: 'Assign', target, value, range: rng(target.range.start, value.range.end) }
  }

  private parseLValue(): AST.LValue {
    const base = this.parsePrimaryChain()
    if (base.kind === 'IdentRef') {
      return { kind: 'LIdent', id: base.id }
    } else if (base.kind === 'Member') {
      return { kind: 'LMember', obj: base.obj, prop: base.prop, range: base.range }
    } else if (base.kind === 'Index') {
      return { kind: 'LIndex', obj: base.obj, index: base.index, range: base.range }
    }
    this.fail('invalid assignment target')
  }

  private parseIf(): AST.IfStmt {
    const t0 = this.take(TT.KwIf)
    const cond = this.parseExpr()
    const then = this.parseBlock()
    let otherwise: AST.Block | undefined
    if (this.match(TT.KwElse)) otherwise = this.parseBlock()
    return { kind: 'IfStmt', cond, then, otherwise, range: rng(t0.range.start, (otherwise ?? then).range.end) }
  }

  private parseSwitch(): AST.SwitchStmt {
    const t0 = this.take(TT.KwSw)
    const disc = this.parseExpr()
    this.take(TT.LBrace)
    const cases: (AST.SwitchCase | AST.SwitchDefault)[] = []
    while (!this.is(TT.RBrace)) {
      this.skipNL()
      if (this.is(TT.RBrace)) break
      if (this.is(TT.KwCase)) {
        const c0 = this.take(TT.KwCase)
        const lab = this.parseLiteral()
        this.take(TT.Colon)
        const body = this.parseBlock()
        cases.push({ kind: 'Case', label: lab, body, range: rng(c0.range.start, body.range.end) })
      } else if (this.is(TT.KwDefault)) {
        const d0 = this.take(TT.KwDefault)
        this.take(TT.Colon)
        const body = this.parseBlock()
        cases.push({ kind: 'Default', body, range: rng(d0.range.start, body.range.end) })
      } else {
        this.fail('expected case/default/}')
      }
      this.skipNL()
    }
    const rb = this.take(TT.RBrace)
    return { kind: 'SwitchStmt', disc, cases, range: rng(t0.range.start, rb.range.end) }
  }

  private parseWhile(): AST.WhileStmt {
    const t0 = this.take(TT.KwWhile)
    const cond = this.parseExpr()
    const body = this.parseBlock()
    return { kind: 'WhileStmt', cond, body, range: rng(t0.range.start, body.range.end) }
  }

  private parseDoWhile(): AST.DoWhileStmt {
    const t0 = this.take(TT.KwDo)
    const body = this.parseBlock()
    this.take(TT.KwWhile)
    const cond = this.parseExpr()
    this.expectNL()
    return { kind: 'DoWhileStmt', body, cond, range: rng(t0.range.start, cond.range.end) }
  }

  private parseFor(): AST.ForStmt | AST.ForInStmt {
    const t0 = this.take(TT.KwFor)
    // peek for 'ident in'
    if (this.is(TT.Identifier) && this.peekKind(1) === TT.KwIn) {
      const it = this.expectIdent()
      this.take(TT.KwIn)
      const src = this.parseExpr()
      const body = this.parseBlock()
      return { kind: 'ForInStmt', it, src, body, range: rng(t0.range.start, body.range.end) }
    }
    // for init , cond , step Block
    const init = this.parseForInit()
    this.take(TT.Comma)
    const cond = this.parseExpr()
    this.take(TT.Comma)
    const step = this.isAssignStarter() ? this.parseAssign() : this.parseExpr()
    const body = this.parseBlock()
    return { kind: 'ForStmt', init, cond, step, body, range: rng(t0.range.start, body.range.end) }
  }

  private parseForInit(): AST.ForInit {
    if (this.isVarDeclStarter()) return this.parseVarDecl()
    return this.parseAssign()
  }

  private parseTry(): AST.TryCatchStmt {
    const t0 = this.take(TT.KwTry)
    const body = this.parseBlock()
    const catches: AST.TryCatch[] = []
    while (this.match(TT.KwCatch)) {
      const ty = this.parseType()
      let id: AST.Identifier | undefined
      if (this.is(TT.Identifier)) id = this.expectIdent()
      const cbody = this.parseBlock()
      catches.push({ type: ty, id, body: cbody, range: rng(ty.range.start, cbody.range.end) })
    }
    if (catches.length === 0) this.fail('try must be followed by at least one catch')
    return { kind: 'TryCatchStmt', body, catches, range: rng(t0.range.start, catches[catches.length - 1].range.end) }
  }

  private parseReturnNL(): AST.ReturnStmt {
    const t0 = this.take(TT.KwReturn)
    let value: AST.Expr | undefined
    if (!isNL(this.cur())) value = this.parseExpr()
    this.expectNL()
    return { kind: 'ReturnStmt', value, range: rng(t0.range.start, this.prevEnd()) }
  }

  // ——— expressions (Pratt) ———

  private parseExpr(): AST.Expr { return this.parseOr() }

  private parseOr(): AST.Expr {
    let left = this.parseAnd()
    while (this.match(TT.OrOr)) {
      const op = '||'
      const r = this.parseAnd()
      left = bin(left, op, r)
    }
    return left
  }
  private parseAnd(): AST.Expr {
    let left = this.parseEq()
    while (this.match(TT.AndAnd)) {
      const op = '&&'
      const r = this.parseEq()
      left = bin(left, op, r)
    }
    return left
  }
  private parseEq(): AST.Expr {
    let left = this.parseRel()
    while (this.is(TT.EqEq) || this.is(TT.BangEq)) {
      const op = this.take().lexeme
      const r = this.parseRel()
      left = bin(left, op, r)
    }
    return left
  }
  private parseRel(): AST.Expr {
    let left = this.parseAdd()
    while (this.is(TT.Lt) || this.is(TT.Lte) || this.is(TT.Gt) || this.is(TT.Gte)) {
      const op = this.take().lexeme
      const r = this.parseAdd()
      left = bin(left, op, r)
    }
    return left
  }
  private parseAdd(): AST.Expr {
    let left = this.parseMul()
    while (this.is(TT.Plus) || this.is(TT.Minus)) {
      const op = this.take().lexeme
      const r = this.parseMul()
      left = bin(left, op, r)
    }
    return left
  }
  private parseMul(): AST.Expr {
    let left = this.parseUnary()
    while (this.is(TT.Star) || this.is(TT.Slash) || this.is(TT.Percent)) {
      const op = this.take().lexeme
      const r = this.parseUnary()
      left = bin(left, op, r)
    }
    return left
  }
  private parseUnary(): AST.Expr {
    if (this.is(TT.Bang) || this.is(TT.Minus) || this.is(TT.Plus) || this.is(TT.Star) || this.is(TT.Amp)) {
      const t = this.take()
      const arg = this.parseUnary()
      return { kind: 'Unary', op: t.lexeme, arg, range: rng(t.range.start, arg.range.end) }
    }
    return this.parsePostfix()
  }

  private parsePostfix(): AST.Expr {
    let expr = this.parsePrimary()
    // chain: call / member / index / ++ --
    for (; ;) {
      if (this.match(TT.LParen)) {
        const args: AST.Expr[] = []
        if (!this.is(TT.RParen)) {
          do { args.push(this.parseExpr()) } while (this.match(TT.Comma))
        }
        const rp = this.take(TT.RParen)
        expr = { kind: 'Call', callee: expr, args, range: rng(expr.range.start, rp.range.end) }
        continue
      }
      if (this.match(TT.Dot)) {
        const prop = this.expectIdent()
        expr = { kind: 'Member', obj: expr, prop, range: rng(expr.range.start, prop.range.end) }
        continue
      }
      if (this.match(TT.LBracket)) {
        const idx = this.parseExpr()
        const rb = this.take(TT.RBracket)
        expr = { kind: 'Index', obj: expr, index: idx, range: rng(expr.range.start, rb.range.end) }
        continue
      }
      if (this.match(TT.PlusPlus) || this.match(TT.MinusMinus)) {
        // Transform as binary with dummy? For now disallow in expressions (can add later)
        this.fail('postfix ++/-- not supported in expressions yet')
      }
      break
    }
    return expr
  }

  private parsePrimary(): AST.Expr {
    // vector literal disambiguation: if we see '<', try to parse a vector literal only if we can find a matching '>' on same line span (no NL)
    if (this.is(TT.Lt) && this.canParseVectorLiteral()) {
      return this.parseVectorLiteral()
    }

    if (this.isLiteralStart()) return this.parseLiteral()
    if (this.is(TT.Identifier)) {
      const id = this.expectIdent()
      return { kind: 'IdentRef', id }
    }
    if (this.match(TT.LParen)) {
      const inner = this.parseExpr()
      const rp = this.take(TT.RParen)
      return { kind: 'Paren', inner, range: rng(inner.range.start, rp.range.end) }
    }
    if (this.match(TT.LBracket)) {
      const start = this.prevStart()
      const elems: AST.Expr[] = []
      if (!this.is(TT.RBracket)) {
        do { elems.push(this.parseExpr()) } while (this.match(TT.Comma))
      }
      const rb = this.take(TT.RBracket)
      return { kind: 'ArrayOrCollection', elems, range: rng(start, rb.range.end) }
    }
    this.fail('expected expression')
  }

  private parseVectorLiteral(): AST.Expr {
    const lt = this.take(TT.Lt)
    const elems: AST.Expr[] = []
    if (!this.is(TT.Gt)) {
      do { elems.push(this.parseExpr()) } while (this.match(TT.Comma))
    }
    const gt = this.take(TT.Gt)
    return { kind: 'Vector', elems, range: rng(lt.range.start, gt.range.end) }
  }

  private parseLiteral(): AST.Literal {
    const t = this.take()
    switch (t.kind) {
      case TT.Int: return { kind: 'Int', value: t.lexeme, range: t.range }
      case TT.KwTrue: return { kind: 'Bool', value: true, range: t.range }
      case TT.KwFalse: return { kind: 'Bool', value: false, range: t.range }
      case TT.Char: return { kind: 'Char', value: t.lexeme, range: t.range }
      case TT.String: return { kind: 'String', value: t.lexeme, range: t.range }
      default: this.fail('expected literal')
    }
  }

  // ——— types ———

  private parseType(): AST.TypeRef {
    return this.parseTypeUnion()
  }
  private parseTypeUnion(): AST.TypeRef {
    let left = this.parseTypeInter()
    while (this.match(TT.Pipe)) {
      const right = this.parseTypeInter()
      left = { kind: 'TypeUnion', left, right, range: rng(left.range.start, right.range.end) }
    }
    return left
  }
  private parseTypeInter(): AST.TypeRef {
    let left = this.parseTypeSimple()
    while (this.match(TT.Amp)) {
      const right = this.parseTypeSimple()
      left = { kind: 'TypeInter', left, right, range: rng(left.range.start, right.range.end) }
    }
    return left
  }
  private parseTypeSimple(): AST.TypeRef {
    if (this.match(TT.LParen)) {
      const params: AST.TypeRef[] = []
      if (!this.is(TT.RParen)) {
        do { params.push(this.parseType()) } while (this.match(TT.Comma))
      }
      this.take(TT.RParen)
      this.take(TT.Arrow)
      const ret = this.parseType()
      const start = params.length ? params[0].range.start : this.prevStart()
      return { kind: 'TypeFunc', params, ret, range: rng(start, ret.range.end) }
    }
    if (this.is(TT.Identifier)) {
      const id = this.expectIdent()
      return { kind: 'TypeIdent', name: id.name, range: id.range }
    }
    if (this.is(TT.KwAny)) {
      const t = this.take(TT.KwAny)
      return { kind: 'TypeAny', range: t.range }
    }
    this.fail('expected type')
  }

  // ——— helpers: starters/guards ———

  private isVarDeclStarter(): boolean {
    return (this.isTypeStart() && this.isAheadIdentAfterType()) || this.is(TT.Identifier) && this.peekKind(1) === TT.Eq
  }
  private isAssignStarter(): boolean {
    // lvalue starts with Identifier / Primary chain; we conservatively allow Identifier here and resolve in parseLValue
    // require an '=' ahead to avoid grabbing expression statements
    let k = 0
    if (this.peekKind(k) !== TT.Identifier) return false
    while (true) {
      const kind = this.peekKind(++k)
      if (kind === TT.Dot && this.peekKind(k + 1) === TT.Identifier) { k += 1; continue }
      if (kind === TT.LBracket) {
        // skip one bracketed expr for index
        // very rough lookahead: find matching ]
        let depth = 1, j = k + 1
        while (depth > 0) {
          const kk = this.peekKind(j++)
          if (kk === TT.LBracket) depth++
          else if (kk === TT.RBracket) depth--
          else if (kk === TT.EOF || kk === TT.NL) break
        }
        k = j - 1
        continue
      }
      break
    }
    return this.peekKind(k) === TT.Eq
  }

  private isTypeStart(): boolean {
    // any identifier or 'any' can start a type; function type starts with '('
    return this.is(TT.Identifier) || this.is(TT.KwAny) || this.is(TT.LParen)
  }
  private isAheadIdentAfterType(): boolean {
    // quick heuristic: parse a single TypeIdent / ( ... )-> and see Identifier next
    if (this.is(TT.Identifier)) return this.peekKind(1) === TT.Identifier
    if (this.is(TT.KwAny)) return this.peekKind(1) === TT.Identifier
    if (this.is(TT.LParen)) {
      // function type: find matching ')' then '->' then a type, then Identifier
      let k = 0, depth = 0
      do {
        const kind = this.peekKind(k++)
        if (kind === TT.LParen) depth++
        else if (kind === TT.RParen) depth--
        else if (kind === TT.EOF || kind === TT.NL) return false
      } while (depth > 0)
      if (this.peekKind(k) !== TT.Arrow) return false
      // skip '->' and assume a simple type token then Identifier
      return this.peekKind(k + 2) === TT.Identifier
    }
    return false
  }

  private isLiteralStart(): boolean {
    return this.is(TT.Int) || this.is(TT.KwTrue) || this.is(TT.KwFalse) || this.is(TT.Char) || this.is(TT.String)
  }

  private canParseVectorLiteral(): boolean {
    // ensure a matching '>' exists before NL/EOF and commas balance brackets/parens
    let k = 1, depth = 1
    while (true) {
      const kind = this.peekKind(k++)
      if (kind === TT.EOF || kind === TT.NL) return false
      if (kind === TT.Lt) depth++
      else if (kind === TT.Gt) { depth--; if (depth === 0) return true }
      else if (kind === TT.LParen || kind === TT.LBracket) depth++
      else if (kind === TT.RParen || kind === TT.RBracket) depth--
    }
  }

  // ——— token API ———

  private cur(): Token { return this.toks[this.i] }
  private is(k: TT): boolean { return this.cur().kind === k }
  private take(k?: TT): Token {
    const t = this.cur()
    if (k && t.kind !== k) this.fail(`expected ${k} but found ${t.kind}`)
    this.i++
    return t
  }
  private match(k: TT): boolean {
    if (this.is(k)) { this.i++; return true }
    return false
  }
  private expectIdent(): AST.Identifier {
    const t = this.take(TT.Identifier)
    return { kind: 'Identifier', name: t.lexeme, range: t.range }
  }
  private expectSym(sym: '::'): void {
    // synthesized from two tokens ':' ':'
    if (!(this.is(TT.Colon) && this.peekKind(1) === TT.Colon)) this.fail("expected '::'")
    this.take(TT.Colon); this.take(TT.Colon)
  }
  private peekKind(ahead: number): TT { return this.toks[this.i + ahead]?.kind ?? TT.EOF }
  private peekSym(s: string): boolean {
    if (s === '::') return this.is(TT.Colon) && this.peekKind(1) === TT.Colon
    return false
  }
  private skipNL(): void { while (this.is(TT.NL)) this.i++ }
  private expectNL(): void {
    if (!this.is(TT.NL)) this.fail('expected newline')
    while (this.is(TT.NL)) this.i++
  }
  private pos(): AST.Pos { return this.cur().range.start }
  private prevEnd(): AST.Pos { return this.toks[this.i - 1].range.end }
  private prevStart(): AST.Pos { return this.toks[this.i - 1].range.start }

  private semanticMainChecks(decls: AST.TopLevel[]): void {
    // enforce: `main` must not declare a return type and must not have `return expr`
    for (const d of decls) {
      if (d.kind === 'FuncDecl' && d.sig.name.name === 'main') {
        if (d.sig.ret) this.errorAt(d.sig.range, 'main must not declare a return type')
        for (const s of d.body.stmts) {
          if (s.kind === 'ReturnStmt' && s.value) this.errorAt(s.range, 'main cannot return a value')
        }
      }
    }
  }

  private errorAt(range: Range, msg: string): never {
    throw new Error(`parse error @ ${range.start.line}:${range.start.col}: ${msg}`)
  }
  private fail(msg: string): never {
    const p = this.pos()
    throw new Error(`parse error @ ${p.line}:${p.col}: ${msg}`)
  }
}

function rng(start: AST.Pos, end: AST.Pos): Range { return { start, end } }
function bin(l: AST.Expr, op: string, r: AST.Expr): AST.Expr {
  return { kind: 'Binary', op, left: l, right: r, range: { start: l.range.start, end: r.range.end } }
}

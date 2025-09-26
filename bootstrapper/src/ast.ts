// src/ast.ts
export type Pos = { index: number; line: number; col: number }
export type Range = { start: Pos; end: Pos }

export type Program = { kind: 'Program', decls: TopLevel[], range: Range }

export type TopLevel =
  | ImportDecl
  | ConstDecl
  | FuncDecl
  | StructDecl
  | MethodDef

export type ImportDecl = {
  kind: 'ImportDecl'
  name: Identifier
  alias?: Identifier
  range: Range
}

export type ConstDecl = {
  kind: 'ConstDecl'
  id: Identifier
  init: Expr
  range: Range
}

export type TypeRef =
  | { kind: 'TypeIdent', name: string, range: Range }
  | { kind: 'TypeAny', range: Range }
  | { kind: 'TypeUnion', left: TypeRef, right: TypeRef, range: Range }
  | { kind: 'TypeInter', left: TypeRef, right: TypeRef, range: Range }
  | { kind: 'TypeFunc', params: TypeRef[], ret: TypeRef, range: Range }

export type Identifier = { kind: 'Identifier', name: string, range: Range }

export type FuncParam = { name: Identifier, type?: TypeRef }
export type FuncSig = { name: Identifier, params: FuncParam[], ret?: TypeRef, range: Range }

export type FuncDecl = {
  kind: 'FuncDecl'
  sig: FuncSig
  body: Block
  range: Range
}

export type MethodDef = {
  kind: 'MethodDef'
  recv: Identifier
  name: Identifier
  params: FuncParam[]
  ret?: TypeRef
  body: Block
  range: Range
}

export type StructField = { pub: boolean, name: Identifier, type: TypeRef, range: Range }
export type StructMember =
  | { kind: 'StructField', field: StructField }
  | { kind: 'StructSig', pub: boolean, sig: FuncSig }

export type StructDecl = {
  kind: 'StructDecl'
  name: Identifier
  members: StructMember[]
  range: Range
}

export type Block = { kind: 'Block', stmts: Stmt[], range: Range }

export type Stmt =
  | VarDecl
  | Assign
  | ExprStmt
  | IfStmt
  | SwitchStmt
  | WhileStmt
  | DoWhileStmt
  | ForStmt
  | ForInStmt
  | TryCatchStmt
  | ReturnStmt
  | BreakStmt
  | ContinueStmt
  | ConstDecl

export type VarDecl = {
  kind: 'VarDecl'
  name: Identifier
  type?: TypeRef
  init: Expr
  range: Range
}

export type Assign = { kind: 'Assign', target: LValue, value: Expr, range: Range }
export type LValue =
  | { kind: 'LIdent', id: Identifier }
  | { kind: 'LMember', obj: Expr, prop: Identifier, range: Range }
  | { kind: 'LIndex', obj: Expr, index: Expr, range: Range }

export type ExprStmt = { kind: 'ExprStmt', expr: Expr, range: Range }

export type IfStmt = {
  kind: 'IfStmt'
  cond: Expr
  then: Block
  otherwise?: Block
  range: Range
}

export type SwitchCase = { kind: 'Case', label: Literal, body: Block, range: Range }
export type SwitchDefault = { kind: 'Default', body: Block, range: Range }
export type SwitchStmt = {
  kind: 'SwitchStmt'
  disc: Expr
  cases: (SwitchCase | SwitchDefault)[]
  range: Range
}

export type WhileStmt = { kind: 'WhileStmt', cond: Expr, body: Block, range: Range }
export type DoWhileStmt = { kind: 'DoWhileStmt', body: Block, cond: Expr, range: Range }

export type ForStmt = {
  kind: 'ForStmt'
  init: ForInit
  cond: Expr
  step: Expr | Assign
  body: Block
  range: Range
}
export type ForInit = VarDecl | Assign

export type ForInStmt = { kind: 'ForInStmt', it: Identifier, src: Expr, body: Block, range: Range }

export type TryCatch = { type: TypeRef, id?: Identifier, body: Block, range: Range }
export type TryCatchStmt = { kind: 'TryCatchStmt', body: Block, catches: TryCatch[], range: Range }

export type ReturnStmt = { kind: 'ReturnStmt', value?: Expr, range: Range }
export type BreakStmt = { kind: 'BreakStmt', range: Range }
export type ContinueStmt = { kind: 'ContinueStmt', range: Range }

export type Literal =
  | { kind: 'Int', value: string, range: Range }
  | { kind: 'Bool', value: boolean, range: Range }
  | { kind: 'Char', value: string, range: Range }
  | { kind: 'String', value: string, range: Range }

export type Expr =
  | Literal
  | { kind: 'IdentRef', id: Identifier }
  | { kind: 'Call', callee: Expr, args: Expr[], range: Range }
  | { kind: 'Member', obj: Expr, prop: Identifier, range: Range }
  | { kind: 'Index', obj: Expr, index: Expr, range: Range }
  | { kind: 'Unary', op: string, arg: Expr, range: Range }
  | { kind: 'Binary', op: string, left: Expr, right: Expr, range: Range }
  | { kind: 'Paren', inner: Expr, range: Range }
  | { kind: 'ArrayOrCollection', elems: Expr[], range: Range }
  | { kind: 'Vector', elems: Expr[], range: Range }

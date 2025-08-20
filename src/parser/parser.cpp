#include <stdexcept>
#include <string>
#include <iostream>
#include "parser/parser.hpp"

using namespace Blam;

/* ------------------------------------------------- helpers */

Parser::Parser(std::shared_ptr<LListNode<Token>> head, const std::string &src)
    : curr(std::move(head)), src(src) {}

Token &Parser::peek()
{
  if (!curr || !curr->data)
  {
    throw std::runtime_error("Unexpected end of token stream.");
  }
  return *curr->data;
}

void Parser::skipWhitespace()
{
  // Advance past tokens that should not influence parsing of expressions.  In
  // addition to plain whitespace and RAW segments (unrecognized text), we
  // also treat newlines as whitespace for the purposes of expression
  // parsing.  This allows multi‑line expressions to be parsed without
  // throwing unexpected errors on newline tokens.
  while (curr && curr->data &&
         (curr->data->type == Type::WHITESPACE || curr->data->type == Type::RAW || curr->data->type == Type::NLINE))
    curr = curr->next;
}

void Parser::advance()
{
  if (curr)
    curr = curr->next;
  skipWhitespace();
}

bool Parser::match(Type t)
{
  skipWhitespace();
  if (curr && curr->data->type == t)
  {
    advance();
    return true;
  }
  return false;
}

/* ------------------------------------------------- precedence + associativity */

int Parser::getPrecedence(Type type)
{
  switch (type)
  {
  case Type::EQEQ:
  case Type::NEQ:
    return 5;
  case Type::LT:
  case Type::LTE:
  case Type::GT:
  case Type::GTE:
    return 10;
  case Type::PLUS:
  case Type::MIN:
    return 20;
  case Type::MULT:
  case Type::DIV:
    return 30;
  case Type::EXP:
    return 40;
  default:
    return -1;
  }
}

bool Parser::isRightAssociative(Type type)
{
  return type == Type::EXP;
}

/* ------------------------------------------------- recursive-descent parser */

std::shared_ptr<Expr> Parser::parseExpression()
{
  auto lhs = parsePrimary();
  return parseBinaryOpRHS(0, lhs);
}

std::shared_ptr<Expr> Parser::parsePrimary()
{
  if (!curr || !curr->data)
    throw std::runtime_error("Unexpected end of input to primary expression.");

  Token &tok = peek();

  if (tok.type == Type::IDENT)
  {
    std::string name = src.substr(tok.pos, tok.len);
    advance();
    return std::make_shared<VariableExpr>(name);
  }

  // Decimal literals should be treated similarly to integers.  Use stod
  // to convert the substring into a floating point value.
  if (tok.type == Type::DECIMAL)
  {
    double value = std::stod(src.substr(tok.pos, tok.len));
    advance();
    return std::make_shared<NumberExpr>(value);
  }

  if (tok.type == Type::NUMBER)
  {
    double value = std::stod(src.substr(tok.pos, tok.len));
    advance();
    return std::make_shared<NumberExpr>(value);
  }

  if (tok.type == Type::SMBRACKET_L)
  {
    advance(); // consume '('
    auto expr = parseExpression();
    if (!match(Type::SMBRACKET_R))
      throw std::runtime_error("Expected closing ')'");
    return expr;
  }

  throw std::runtime_error("Unknown token in expression: " + typeToString(tok.type));
}

std::shared_ptr<Expr> Parser::parseBinaryOpRHS(int minPrec, std::shared_ptr<Expr> lhs)
{
  while (true)
  {
    // Always land on a real operator (skip whitespace/newlines/raw, etc.)
    skipWhitespace();

    // Current operator and its precedence
    Type opType = (curr && curr->data) ? curr->data->type : Type::END;
    int opPrec = getPrecedence(opType);
    if (opPrec < minPrec)
      break; // nothing more to bind at this precedence

    // Capture operator lexeme directly from source (robust against enum->string drift)
    const auto &tok = *curr->data;
    std::string opSymbol = src.substr(tok.pos, tok.len);

    // Consume operator
    curr = curr->next;

    // Parse RHS primary
    skipWhitespace();
    auto rhs = parsePrimary();

    // After parsing RHS, decide if we should nest it with a higher (or right-assoc equal) precedence operator
    while (true)
    {
      skipWhitespace();
      Type nextType = (curr && curr->data) ? curr->data->type : Type::END;
      int nextPrec = getPrecedence(nextType);
      if (nextPrec < 0)
        break;

      // If the next operator binds tighter than the current operator,
      // or binds equally and is right-associative, let it consume from RHS.
      if (opPrec < nextPrec || (opPrec == nextPrec && isRightAssociative(opType)))
      {
        // For right-associative ops, equal precedence should NOT force +1.
        int nextMin = opPrec + (isRightAssociative(opType) ? 0 : 1);
        rhs = parseBinaryOpRHS(nextMin, rhs);
      }
      break;
    }

    // Build/accumulate: (lhs op rhs)
    lhs = std::make_shared<BinaryExpr>(opSymbol, lhs, rhs);
  }

  return lhs;
}

/* ------------------------------------------------- top-level */

std::vector<std::shared_ptr<Expr>> Parser::parse()
{
  std::vector<std::shared_ptr<Expr>> ast;
  skipWhitespace();
  while (curr && curr->data && curr->data->type != Type::END)
  {
    ast.push_back(parseExpression());
  }
  return ast;
}

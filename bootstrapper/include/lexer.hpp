// lexer.hpp
#pragma once
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include "token.hpp" // contains Type, ORDER, type_to_string, regex_for_type, Token
#include "llist.hpp" // contains LList<T>, LListNode<T>

// Split a RAW token into smaller tokens based on the regex for `ttype`.
// Returns the head of the replacement chain (or the original token if no split).
inline LListNode<Token> *process_token(LListNode<Token> *token, Type ttype, std::string_view src)
{
  if (!token || token->data.type != Type::RAW)
    return token;

  const std::size_t pos = token->data.pos;
  const std::size_t length = token->data.length;

  const char *pat_cstr = regex_for_type(ttype);
  if (!pat_cstr || *pat_cstr == '\0')
    return token;

  const std::string pattern{pat_cstr};
  const std::string segment{src.substr(pos, length)}; // std::regex works on std::string
  const std::regex re(pattern, std::regex::ECMAScript | std::regex::optimize);

  std::sregex_iterator it(segment.begin(), segment.end(), re);
  std::sregex_iterator end;

  if (it == end)
    return token; // no matches → keep original

  LListNode<Token> *head = nullptr;
  LListNode<Token> *tail = nullptr;
  std::size_t cursor = pos;

  for (; it != end; ++it)
  {
    const auto &m = *it;
    const std::size_t match_start = pos + static_cast<std::size_t>(m.position());
    const std::size_t match_len = static_cast<std::size_t>(m.length());

    // leading RAW (between cursor and match_start)
    if (cursor < match_start)
    {
      auto *raw = new LListNode<Token>(Token{Type::RAW, cursor, match_start - cursor});
      if (!head)
        head = raw;
      else
      {
        tail->next = raw;
        raw->prev = tail;
      }
      tail = raw;
    }

    // the matched token
    {
      auto *tok = new LListNode<Token>(Token{ttype, match_start, match_len});
      if (!head)
        head = tok;
      else
      {
        tail->next = tok;
        tok->prev = tail;
      }
      tail = tok;
    }

    cursor = match_start + match_len;
  }

  // trailing RAW
  if (cursor < pos + length)
  {
    auto *raw = new LListNode<Token>(Token{Type::RAW, cursor, (pos + length) - cursor});
    if (!head)
      head = raw;
    else
    {
      tail->next = raw;
      raw->prev = tail;
    }
    tail = raw;
  }

  return head;
}

// Tokenize the whole source into a linked list of Token nodes.
inline LList<Token> tokenize(std::string_view src)
{
  LList<Token> lst;
  lst.head = new LListNode<Token>(Token{Type::RAW, 0, src.size()});

  // Walk types in precedence order; split RAW nodes by each pattern.
  for (Type ttype : ORDER)
  {
    if (ttype == Type::SKIP || ttype == Type::END || ttype == Type::RAW)
      continue;

    auto *curr = lst.head;
    while (curr)
    {
      auto *nxt = curr->next; // cache next before possible splice
      if (curr->data.type == Type::RAW)
      {
        LListNode<Token> *repl = process_token(curr, ttype, src);
        if (repl != curr)
        {
          lst.replace(curr, repl); // splice in the chain
          // optional: delete curr;  // if the list owns nodes, free the victim
        }
      }
      curr = nxt;
    }
  }

  // Drop WHITESPACE tokens
  for (auto *node = lst.head; node;)
  {
    auto *nxt = node->next;
    if (node->data.type == Type::WHITESPACE)
    {
      if (node->prev)
        node->prev->next = node->next;
      else
        lst.head = node->next;
      if (node->next)
        node->next->prev = node->prev;
      // optional: delete node;
    }
    node = nxt;
  }

  return lst;
}

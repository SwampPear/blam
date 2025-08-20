#include <iostream>
#include <sstream>
#include <regex>

#include "tokenizer/tokenizer.hpp"

namespace Blam
{

  std::shared_ptr<LListNode<Token>> processToken(std::shared_ptr<LListNode<Token>> token, Type type, const std::string &src)
  {
    uint16_t pos = token->data->pos;
    uint16_t len = token->data->len;

    std::regex reg(TOKEN_EXPR[type]);
    auto begin = std::sregex_iterator(src.begin() + pos, src.begin() + pos + len, reg);
    auto end = std::sregex_iterator();

    if (begin == end)
      return token; // No matches, return original RAW node

    std::shared_ptr<LListNode<Token>> head = nullptr;
    std::shared_ptr<LListNode<Token>> tail = nullptr;

    uint16_t cursor = pos;

    for (auto it = begin; it != end; ++it)
    {
      const std::smatch &match = *it;
      uint16_t matchStart = static_cast<uint16_t>(match.position() + pos);
      uint16_t matchLen = static_cast<uint16_t>(match.length());

      // Add RAW before match
      if (cursor < matchStart)
      {
        auto raw = std::make_shared<LListNode<Token>>();
        raw->data = std::make_shared<Token>();
        raw->data->type = Type::RAW;
        raw->data->pos = cursor;
        raw->data->len = static_cast<uint16_t>(matchStart - cursor);

        // auto expanded = processToken(raw, type, src);
        if (!head)
          head = raw;
        else
          tail->next = raw, raw->prev = tail;

        while (raw->next)
          raw = raw->next;
        tail = raw;
      }

      // Add matched token
      auto tok = std::make_shared<LListNode<Token>>();
      tok->data = std::make_shared<Token>();
      tok->data->type = type;
      tok->data->pos = matchStart;
      tok->data->len = matchLen;

      if (!head)
        head = tok;
      else
        tail->next = tok, tok->prev = tail;
      tail = tok;

      cursor = matchStart + matchLen;
    }

    // Add final RAW after last match, if any
    if (cursor < pos + len)
    {
      auto raw = std::make_shared<LListNode<Token>>();
      raw->data = std::make_shared<Token>();
      raw->data->type = Type::RAW;
      raw->data->pos = cursor;
      raw->data->len = static_cast<uint16_t>((pos + len) - cursor);

      // auto expanded = processToken(raw, type, src);
      tail->next = raw;
      raw->prev = tail;

      while (raw->next)
        raw = raw->next;
      tail = raw;
    }

    return head;
  }

  std::shared_ptr<LList<Token>> tokenize(const std::string &src)
  {
    // start with whole source in a single RAW node
    LList<Token> list{};
    list.head = std::make_shared<LListNode<Token>>();
    list.head->data = std::make_shared<Token>();
    list.head->data->type = Type::RAW;
    list.head->data->pos = 0;
    list.head->data->len = static_cast<uint16_t>(src.length());

    // walk through enum values in precedence order
    for (int i = static_cast<int>(Type::MLINE_COMMENT);
         i <= static_cast<int>(Type::MULT); ++i)
    {
      auto a = list.head;
      while (a)
      {
        /// std::cout << typeToString(a->data->type) << std::endl;
        a = a->next;
      }

      Type type = static_cast<Type>(i);

      // iterate over the current linked list
      for (auto curr = list.head; curr;)
      {
        auto next = curr->next; // save pointer now

        if (curr->data->type == Type::RAW)
        {
          // split this RAW node once with the current pattern
          auto replaced = processToken(curr, type, src);

          if (replaced != curr) // something changed
          {
            list.replace(curr, replaced); // splice new nodes in

            // find the last node we just inserted
            auto last = replaced;
            while (last->next)
              last = last->next;

            next = last->next; // continue after them
          }
        }
        curr = next;
      }
    }

    // drop whitespace tokens
    for (auto node = list.head; node;)
    {
      if (node->data && (node->data->type == Type::WHITESPACE))
      {
        auto prev = node->prev;
        auto nxt = node->next;
        if (prev)
          prev->next = nxt;
        else
          list.head = nxt;
        if (nxt)
          nxt->prev = prev;
        node = nxt;
      }
      else
        node = node->next;
    }

    return std::make_shared<LList<Token>>(list);
  }

} // namespace Blam
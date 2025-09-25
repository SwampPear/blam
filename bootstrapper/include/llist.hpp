#pragma once
#include <utility>

template <typename T>
struct LListNode
{
  LListNode *next{nullptr};
  LListNode *prev{nullptr};
  T data{};

  LListNode() = default;
  explicit LListNode(const T &v) : data(v) {}
  explicit LListNode(T &&v) : data(std::move(v)) {}
};

template <typename T>
class LList
{
public:
  LListNode<T> *head{nullptr};

  void replace(LListNode<T> *victim, LListNode<T> *replacement)
  {
    if (!victim || !replacement)
      return;

    // find tail of replacement
    LListNode<T> *repl_tail = replacement;
    while (repl_tail->next)
      repl_tail = repl_tail->next;

    // link replacement to victim's previous
    replacement->prev = victim->prev;
    if (victim->prev)
    {
      victim->prev->next = replacement;
    }
    else
    {
      head = replacement;
    }

    // link replacement to victim's next
    repl_tail->next = victim->next;
    if (victim->next)
    {
      victim->next->prev = repl_tail;
    }

    // disconnect victim
    victim->next = nullptr;
    victim->prev = nullptr;
  }
};

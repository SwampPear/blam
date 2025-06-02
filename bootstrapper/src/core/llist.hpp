#pragma once

#include <memory>

namespace Blam {

template <typename T>
struct LListNode {
    std::shared_ptr<LListNode<T>> next;
    std::shared_ptr<LListNode<T>> prev;

    std::shared_ptr<T> data;
};

template <typename T>
struct LList {
    std::shared_ptr<LListNode<T>> head;

    void replace(std::shared_ptr<LListNode<T>> victim, std::shared_ptr<LListNode<T>> replacement) {
        if (!victim || !replacement) return;

        // link prev
        replacement->prev = victim->prev;
        if (victim->prev) {
            victim->prev->next = replacement;
        } else {
            head = std::move(replacement);
        }

        // link next
        replacement->next = victim->next;
        if (victim->next) {
            victim->next->prev = replacement;
        }
    }
};

}  // namespace Blam
#include "core/llist.hpp"


namespace Blam {

template <typename T>
void LList<T>::replace(std::unique_ptr<LListNode<T>> victim, std::unique_ptr<LListNode<T>> replacement) {
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

}  // namespace Blam
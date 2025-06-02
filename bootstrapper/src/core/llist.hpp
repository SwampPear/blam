#pragma once

#include <memory>

namespace Blam {

template <typename T>
struct LListNode {
    std::shared_ptr<T> next;
    std::shared_ptr<T> prev;

    T data;
};

template <typename T>
struct LList {
    using pLListNode = std::unique_ptr<LListNode<T>>;
    
    pLListNode head;

    void replace(pLListNode victim, pLListNode range);
};

}  // namespace Blam
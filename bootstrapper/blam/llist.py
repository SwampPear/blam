from __future__ import annotations
from typing import Generic, Optional, TypeVar

T = TypeVar("T")


class LListNode(Generic[T]):
    def __init__(self, data: Optional[T] = None):
        self.next: Optional[LListNode[T]] = None
        self.prev: Optional[LListNode[T]] = None
        self.data: Optional[T] = data


class LList(Generic[T]):
    def __init__(self):
        self.head: Optional[LListNode[T]] = None

    def replace(self, victim: Optional[LListNode[T]], replacement: Optional[LListNode[T]]) -> None:
        if not victim or not replacement:
            return

        # Find tail of replacement
        repl_tail = replacement
        while repl_tail.next:
            repl_tail = repl_tail.next

        # Link replacement to victim's neighbors
        replacement.prev = victim.prev
        if victim.prev:
            victim.prev.next = replacement
        else:
            self.head = replacement

        repl_tail.next = victim.next
        if victim.next:
            victim.next.prev = repl_tail

        # Disconnect victim
        victim.next = None
        victim.prev = None

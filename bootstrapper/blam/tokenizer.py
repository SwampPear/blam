import re
from typing import Optional
from blam.llist import LListNode, LList
from blam.token import Token, Type, TOKEN_EXPR, ORDER

# Assuming Type, TOKEN_EXPR, Token, LList, LListNode are already defined
# (from your previous refactors)


def process_token(token: LListNode[Token], ttype: Type, src: str) -> LListNode[Token]:
    """Split a RAW token into smaller tokens based on regex for ttype."""
    pos = token.data.pos
    length = token.data.length

    pattern = TOKEN_EXPR.get(ttype)
    if not pattern:
        return token

    matches = list(re.finditer(pattern, src[pos:pos + length]))
    if not matches:
        return token  # no matches, return original

    head: Optional[LListNode[Token]] = None
    tail: Optional[LListNode[Token]] = None
    cursor = pos

    for match in matches:
        match_start = pos + match.start()
        match_len = match.end() - match.start()

        # Add RAW before the match
        if cursor < match_start:
            raw = LListNode(Token(Type.RAW, cursor, match_start - cursor))
            if not head:
                head = raw
            else:
                tail.next = raw
                raw.prev = tail
            tail = raw

        # Add the matched token
        tok = LListNode(Token(ttype, match_start, match_len))
        if not head:
            head = tok
        else:
            tail.next = tok
            tok.prev = tail
        tail = tok

        cursor = match_start + match_len

    # Add trailing RAW
    if cursor < pos + length:
        raw = LListNode(Token(Type.RAW, cursor, (pos + length) - cursor))
        if not head:
            head = raw
        else:
            tail.next = raw
            raw.prev = tail
        tail = raw

    return head


def tokenize(src: str) -> LList[Token]:
    """Tokenize a source string into a linked list of Token nodes."""
    lst: LList[Token] = LList()
    lst.head = LListNode(Token(Type.RAW, 0, len(src)))

    # Walk through token types in precedence order
    for ttype in ORDER:
        if ttype in (Type.SKIP, Type.END, Type.RAW):
            continue

        curr = lst.head
        while curr:
            nxt = curr.next
            if curr.data.type == Type.RAW:
                replaced = process_token(curr, ttype, src)
                if replaced is not curr:  # if a split happened
                    lst.replace(curr, replaced)
            curr = nxt

    # Drop whitespace tokens
    node = lst.head
    while node:
        nxt = node.next
        if node.data and node.data.type == Type.WHITESPACE:
            if node.prev:
                node.prev.next = node.next
            else:
                lst.head = node.next
            if node.next:
                node.next.prev = node.prev
        node = nxt

    return lst

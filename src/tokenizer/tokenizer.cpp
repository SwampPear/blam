#include <iostream>
#include <sstream>
#include <regex>


#include "tokenizer/tokenizer.hpp"

namespace Blam {

std::shared_ptr<LListNode<Token>> processToken(std::shared_ptr<LListNode<Token>> token, Type type, const std::string& src) {
    // bounds
    uint16_t pos = token->data->pos;
    uint16_t len = token->data->len;

    // build new list 
    std::shared_ptr<LListNode<Token>> head = nullptr;
    std::shared_ptr<LListNode<Token>> tail = nullptr;

    // match regex
    std::regex reg(TOKEN_EXPR[type]);
    auto it = std::sregex_iterator(src.begin() + pos, src.begin() + pos + len, reg);
    auto regex_end = std::sregex_iterator();
    if (it == regex_end) return token;     // no matches

    uint16_t lastPos = pos;
    for (; it != regex_end; ++it) {
        const std::smatch& match = *it;
        uint16_t matchStart = static_cast<uint16_t>(match.position() + pos);
        uint16_t matchLen   = static_cast<uint16_t>(match.length());

        // add raw segment before match
        if (matchStart > lastPos) {
            auto raw = std::make_shared<LListNode<Token>>();
            raw->data = std::make_shared<Token>();
            raw->data->type = Type::RAW;
            raw->data->pos = lastPos;
            raw->data->len = static_cast<uint16_t>(matchStart - lastPos);

            if (!head) head = raw;
            else tail->next = raw, raw->prev = tail;
            tail = raw;
        }

        // add matched token
        auto tok = std::make_shared<LListNode<Token>>();
        tok->data = std::make_shared<Token>();
        tok->data->type = type;
        tok->data->pos = matchStart;
        tok->data->len = matchLen;

        if (!head) head = tok;
        else tail->next = tok, tok->prev = tail;
        tail = tok;

        lastPos = matchStart + matchLen;
    }

    // raw token after last match
    if (lastPos < pos + len) {
        auto raw = std::make_shared<LListNode<Token>>();
        raw->data = std::make_shared<Token>();
        raw->data->type = Type::RAW;
        raw->data->pos = lastPos;
        raw->data->len = static_cast<uint16_t>((pos + len) - lastPos);

        tail->next = raw;
        raw->prev = tail;
        tail = raw;
    }

    return head;
}

std::shared_ptr<LList<Token>> tokenize(const std::string& src) {
    LList<Token> list{};
    list.head = std::make_shared<LListNode<Token>>();
    list.head->data = std::make_shared<Token>();
    list.head->data->type = Type::RAW;
    list.head->data->pos = 0;
    list.head->data->len = static_cast<uint16_t>(src.length());

    // loop over each token type in order
    for (int i = static_cast<int>(Type::RAW); i <= static_cast<int>(Type::MULT); ++i) {
        Type type = static_cast<Type>(i);
        if (type == Type::RAW || TOKEN_EXPR.find(type) == TOKEN_EXPR.end()) continue;

        std::shared_ptr<LListNode<Token>> curr = list.head;
        while (curr) {
            // save next pointer before possible replacement
            std::shared_ptr<LListNode<Token>> next = curr->next;

            if (curr->data->type == Type::RAW) {
                std::shared_ptr<LListNode<Token>> processed = processToken(curr, type, src);

                // only replace if it actually changed something
                if (processed != curr) {
                    list.replace(curr, processed);

                    // advance to node after replacement
                    std::shared_ptr<LListNode<Token>> last = processed;
                    while (last->next) last = last->next;
                    curr = last->next;
                    continue;  // already updated curr
                }
            }

            curr = next;
        }
    }

    return std::make_shared<LList<Token>>(list);
}

}  // namespace Blam
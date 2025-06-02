#include <iostream>
#include <sstream>
#include <regex>

#include "tokenizer/tokenizer.hpp"

namespace Blam {

std::shared_ptr<Token> processRawToken(std::shared_ptr<Token> victim, Type type, const std::string& input) {
    // bounds
    uint16_t pos = victim->pos;
    uint16_t len = victim->len;

    // build new list 
    std::shared_ptr<Token> head = nullptr;
    std::shared_ptr<Token> tail = nullptr;

    // match regex
    std::regex reg(TOKEN_EXPR[type]);
    auto it = std::sregex_iterator(src.begin() + pos, src.begin() + pos + len, reg);
    auto regex_end = std::sregex_iterator();
    if (it == regex_end) return victim;     // no matches

    uint16_t lastPos = pos;
    for (; it != regex_end; ++it) {
        const std::smatch& match = *it;
        uint16_t matchStart = static_cast<uint16_t>(match.position() + pos);
        uint16_t matchLen   = static_cast<uint16_t>(match.length());

        // add raw segment before match
        if (matchStart > lastPos) {
            auto raw = std::make_shared<Token>();
            raw->type = Type::RAW;
            raw->pos = lastPos;
            raw->len = static_cast<uint16_t>(matchStart - lastPos);

            if (!head) head = raw;
            else tail->next = raw, raw->prev = tail;
            tail = raw;
        }

        // add matched token
        auto tok = std::make_shared<Token>();
        tok->type = type;
        tok->pos = matchStart;
        tok->len = matchLen;

        if (!head) head = tok;
        else tail->next = tok, tok->prev = tail;
        tail = tok;

        lastPos = matchStart + matchLen;
    }

    // raw token after last match
    if (lastPos < pos + len) {
        auto raw = std::make_shared<Token>();
        raw->type = Type::RAW;
        raw->pos = lastPos;
        raw->len = static_cast<uint16_t>((pos + len) - lastPos);

        tail->next = raw;
        raw->prev = tail;
        tail = raw;
    }

    // splice range in place of victim
    if (victim->prev) {
        victim->prev->next = head;
        head->prev = victim->prev;
    }

    if (victim->next) {
        victim->next->prev = tail;
        tail->next = victim->next;
    }

    return (victim->prev ? head : head); // return real head
}

std::unique_ptr<LList<Token>> tokenize(const std::string& src) {
    auto list = std::make_unique<LList<Token>>();

    // initialize head
    list->head = std::make_unique<Token>();
    list->head->type = Type::RAW;
    list->head->pos = 0;
    list->head->len = static_cast<uint16_t>(src.length());

    // loop over each token type
    for (int i = static_cast<int>(Type::RAW); i <= static_cast<int>(Type::MULT); ++i) {
        Type type = static_cast<Type>(i);
        if (type == Type::RAW || TOKEN_EXPR[type].empty()) continue;

        // loop over all raw nodes and process
        auto curr = list->head;
        while (curr) {
            if (curr->type == Type::RAW) {
                auto next = curr->next;

                auto processed = processRawToken(curr, type, src);
                list.replace(curr, processed);

                curr = next;
            } else {
                curr = curr->next;
            }
        }
    }

    return list;
}

}  // namespace Blam
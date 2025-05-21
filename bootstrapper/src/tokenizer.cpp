#include <iostream>
#include "tokenizer.hpp"
#include "utils.hpp"
#include <regex>

namespace Tokenizer {


void printTokens(std::shared_ptr<Token> head, const std::string& input) {
    auto curr = head;
    while (curr != nullptr) {
        int type = static_cast<int>(curr->type);
        uint16_t pos = curr->pos;
        uint16_t len = curr->len;
        std::string content = input.substr(pos, len);

        std::cout << "Type: " << type << ", Pos: " << pos << ", Len: " << len 
                  << ", Content: " << std::endl << content << std::endl;

        curr = curr->next;
    }
}


std::shared_ptr<Token> insertToken(std::shared_ptr<Token> victim, std::shared_ptr<Token> first) {
    auto last = first;
    while (last->next) last = last->next;

    auto prev = victim->prev;
    auto next = victim->next;

    if (prev) {
        prev->next = first;
        first->prev = prev;
    }

    if (next) {
        next->prev = last;
        last->next = next;
    }

    return first; // new head of the sublist
}

std::shared_ptr<Token> processRawToken(std::shared_ptr<Token> victim, Type type, const std::string& input) {
    uint16_t pos = victim->pos;
    uint16_t len = victim->len;

    std::regex reg(tokenExpression[type]);
    auto begin = input.begin() + pos;
    auto end = begin + len;

    auto it = std::sregex_iterator(begin, end, reg);
    auto regex_end = std::sregex_iterator();

    if (it == regex_end) return victim; // no matches

    std::shared_ptr<Token> head = nullptr;
    std::shared_ptr<Token> tail = nullptr;

    uint16_t lastPos = pos;

    for (; it != regex_end; ++it) {
        const std::smatch& match = *it;
        uint16_t matchStart = static_cast<uint16_t>(match.position() + pos);
        uint16_t matchLen   = static_cast<uint16_t>(match.length());

        // Add raw segment before match
        if (matchStart > lastPos) {
            auto raw = std::make_shared<Token>();
            raw->type = Type::RAW;
            raw->pos = lastPos;
            raw->len = static_cast<uint16_t>(matchStart - lastPos);

            if (!head) head = raw;
            else tail->next = raw, raw->prev = tail;
            tail = raw;
        }

        // Add matched token
        auto tok = std::make_shared<Token>();
        tok->type = type;
        tok->pos = matchStart;
        tok->len = matchLen;

        if (!head) head = tok;
        else tail->next = tok, tok->prev = tail;
        tail = tok;

        lastPos = matchStart + matchLen;
    }

    // Raw token after last match
    if (lastPos < pos + len) {
        auto raw = std::make_shared<Token>();
        raw->type = Type::RAW;
        raw->pos = lastPos;
        raw->len = static_cast<uint16_t>((pos + len) - lastPos);

        tail->next = raw;
        raw->prev = tail;
        tail = raw;
    }

    // Splice [head, tail] in place of victim
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



std::shared_ptr<Token> tokenize(const std::string& input) {
    auto head = std::make_shared<Token>();
    head->type = Type::RAW;
    head->pos = 0;
    head->len = static_cast<uint16_t>(input.length());

    for (int i = static_cast<int>(Type::RAW); i <= static_cast<int>(Type::KEYWORD); ++i) {
        Type type = static_cast<Type>(i);
        if (type == Type::RAW || tokenExpression[type].empty()) continue;

        auto curr = head;
        while (curr) {
            if (curr->type == Type::RAW) {
                auto next = curr->next;
                auto newHead = processRawToken(curr, type, input);
                if (curr == head) head = newHead;
                curr = next;
            } else {
                curr = curr->next;
            }
        }
    }

    printTokens(head, input);

    return head;
}


std::shared_ptr<Token> tokenizeFile(const std::string& fp) {
    const std::string contents = Blam::readFile(fp);
    return tokenize(contents);
}

}
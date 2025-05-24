#include <iostream>
#include <sstream>
#include "tokenizer.hpp"
#include "utils.hpp"
#include <regex>

namespace Tokenizer {

std::string toString(std::shared_ptr<Token> token, const std::string& src) {
    std::ostringstream oss;
    oss << "Type: " << static_cast<int>(token->type)
        << ", Pos: " << token->pos
        << ", Len: " << token->len
        << ", Content: " << std::endl
        << src.substr(token->pos, token->len) << std::endl;

    return oss.str();
}

void printTokens(std::shared_ptr<Token> head, const std::string& src) {
    auto curr = head;
    while (curr != nullptr) {
        std::cout << toString(curr, src);
        curr = curr->next;
    }
}

void printTokens(const std::vector<Token>& tokens, const std::string& src) {
    for (const auto& token : tokens) {
        std::cout << toString(token, src);
    }
}

std::shared_ptr<Token> insertToken(std::shared_ptr<Token> victim, std::shared_ptr<Token> first) {
    // get range
    auto last = first;
    while (last->next) last = last->next;

    // prev state
    auto prev = victim->prev;
    auto next = victim->next;

    // connect prev
    if (prev) {
        prev->next = first;
        first->prev = prev;
    }

    // connect next
    if (next) {
        next->prev = last;
        last->next = next;
    }

    // returned in case of head switching
    return first;
}

std::shared_ptr<Token> processRawToken(std::shared_ptr<Token> victim, Type type, const std::string& input) {
    // get bounds
    uint16_t pos = victim->pos;
    uint16_t len = victim->len;

    // build regex
    std::regex reg(TOKEN_EXPR[type]);
    auto it = std::sregex_iterator(input.begin() + pos, input.begin() + pos + len, reg);
    auto regex_end = std::sregex_iterator();

    // no matches
    if (it == regex_end) return victim;

    // build new list 
    std::shared_ptr<Token> head = nullptr;
    std::shared_ptr<Token> tail = nullptr;

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

std::shared_ptr<Token> tokenize(const std::string& input) {
    // root file token
    auto head = std::make_shared<Token>();
    head->type = Type::RAW;
    head->pos = 0;
    head->len = static_cast<uint16_t>(input.length());

    // loop over each lexeme type
    for (int i = static_cast<int>(Type::RAW); i <= static_cast<int>(Type::RET); ++i) {
        Type type = static_cast<Type>(i);
        if (type == Type::RAW || TOKEN_EXPR[type].empty()) continue;

        // loop over all raw nodes and process
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

    return head;
}

std::vector<Token> tokenizeFile(const std::string& fp) {
    const std::string contents = Utils::readFile(fp);
    std::shared_ptr<Token> tokens = tokenize(contents);

    std::vector<Token> tokenVector;
    for (auto curr = tokens; curr; curr = curr->next) {
        tokenVector.push_back(*curr);
    }

    return tokenVector;
}

}  // namespace Tokenizer
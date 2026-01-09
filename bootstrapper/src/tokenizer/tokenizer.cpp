#include <iostream>
#include <sstream>
#include <regex>


#include "tokenizer/tokenizer.hpp"

namespace Blam {

namespace {

std::shared_ptr<LListNode<Token>> buildNode(Type type, size_t pos, size_t len) {
    auto node = std::make_shared<LListNode<Token>>();
    node->data = std::make_shared<Token>();
    node->data->type = type;
    node->data->pos = pos;
    node->data->len = len;
    return node;
}

std::shared_ptr<LListNode<Token>> processMultilineComment(
    std::shared_ptr<LListNode<Token>> token,
    const std::string& src
) {
    size_t pos = token->data->pos;
    size_t len = token->data->len;
    size_t end = pos + len;

    std::shared_ptr<LListNode<Token>> head = nullptr;
    std::shared_ptr<LListNode<Token>> tail = nullptr;

    size_t lastPos = pos;
    size_t searchPos = pos;
    while (searchPos < end) {
        size_t start = src.find("#*", searchPos);
        if (start == std::string::npos || start >= end) break;
        size_t close = src.find("*#", start + 2);
        if (close == std::string::npos || close + 2 > end) break;

        if (start > lastPos) {
            auto raw = buildNode(Type::RAW, lastPos, start - lastPos);
            if (!head) head = raw;
            else tail->next = raw, raw->prev = tail;
            tail = raw;
        }

        auto comment = buildNode(Type::MLINE_COMMENT, start, (close + 2) - start);
        if (!head) head = comment;
        else tail->next = comment, comment->prev = tail;
        tail = comment;

        lastPos = close + 2;
        searchPos = lastPos;
    }

    if (lastPos < end) {
        auto raw = buildNode(Type::RAW, lastPos, end - lastPos);
        if (!head) head = raw;
        else tail->next = raw, raw->prev = tail;
        tail = raw;
    }

    return head ? head : token;
}

}  // namespace

std::shared_ptr<LListNode<Token>> processToken(std::shared_ptr<LListNode<Token>> token, Type type, const std::string& src) {
    if (type == Type::MLINE_COMMENT) {
        return processMultilineComment(token, src);
    }

    size_t pos = token->data->pos;
    size_t len = token->data->len;

    std::shared_ptr<LListNode<Token>> head = nullptr;
    std::shared_ptr<LListNode<Token>> tail = nullptr;

    std::regex reg(TOKEN_EXPR[type]);
    auto it = std::sregex_iterator(src.begin() + pos, src.begin() + pos + len, reg);
    auto regex_end = std::sregex_iterator();
    if (it == regex_end) return token;

    size_t lastPos = pos;
    for (; it != regex_end; ++it) {
        const std::smatch& match = *it;
        size_t matchStart = static_cast<size_t>(match.position() + pos);
        size_t matchLen = static_cast<size_t>(match.length());

        if (matchStart > lastPos) {
            auto raw = buildNode(Type::RAW, lastPos, matchStart - lastPos);
            if (!head) head = raw;
            else tail->next = raw, raw->prev = tail;
            tail = raw;
        }

        auto tok = buildNode(type, matchStart, matchLen);
        if (!head) head = tok;
        else tail->next = tok, tok->prev = tail;
        tail = tok;

        lastPos = matchStart + matchLen;
    }

    if (lastPos < pos + len) {
        auto raw = buildNode(Type::RAW, lastPos, (pos + len) - lastPos);
        tail->next = raw;
        raw->prev = tail;
        tail = raw;
    }

    return head;
}

std::shared_ptr<LList<Token>> tokenize(const std::string& src) {

    // init list
    LList<Token> list{};
    list.head = std::make_shared<LListNode<Token>>();
    list.head->data = std::make_shared<Token>();
    list.head->data->type = Type::RAW;
    list.head->data->pos = 0;
    list.head->data->len = src.length();

    // loop over each token type
    for (int i = static_cast<int>(Type::RAW); i <= static_cast<int>(Type::MULT); ++i) {
        Type type = static_cast<Type>(i);
        if (type == Type::RAW || TOKEN_EXPR[type].empty()) continue;

        // loop over all raw nodes and process
        std::shared_ptr<LListNode<Token>> curr = list.head;
        while (curr) {
            if (curr->data->type == Type::RAW) {
                std::shared_ptr<LListNode<Token>> next = curr->next;

                std::shared_ptr<LListNode<Token>> processed = processToken(curr, type, src);
                if (processed != curr) {
                    list.replace(curr, processed);
                }

                curr = next;
            } else {
                curr = curr->next;
            }
        }
    }

    return std::make_shared<LList<Token>>(list);
}

}  // namespace Blam

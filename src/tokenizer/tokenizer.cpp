#include <iostream>
#include <sstream>
#include <regex>


#include "tokenizer/tokenizer.hpp"

namespace Blam {

    std::shared_ptr<LListNode<Token>> processToken(std::shared_ptr<LListNode<Token>> token, Type type, const std::string& src) {
        uint16_t pos = token->data->pos;
        uint16_t len = token->data->len;
    
        std::shared_ptr<LListNode<Token>> head = nullptr;
        std::shared_ptr<LListNode<Token>> tail = nullptr;
    
        std::regex reg(TOKEN_EXPR[type]);
        auto it = std::sregex_iterator(src.begin() + pos, src.begin() + pos + len, reg);
        auto regex_end = std::sregex_iterator();
        if (it == regex_end) return token;
    
        uint16_t lastPos = pos;
        for (; it != regex_end; ++it) {
            const std::smatch& match = *it;
            uint16_t matchStart = static_cast<uint16_t>(match.position() + pos);
            uint16_t matchLen = static_cast<uint16_t>(match.length());
    
            // Add raw segment before match
            if (matchStart > lastPos) {
                auto raw = std::make_shared<LListNode<Token>>();
                raw->data = std::make_shared<Token>();
                raw->data->type = Type::RAW;
                raw->data->pos = lastPos;
                raw->data->len = static_cast<uint16_t>(matchStart - lastPos);
    
                // ⬇ Recursively process nested RAW token
                auto expanded = processToken(raw, type, src);
    
                if (!head) head = expanded;
                else tail->next = expanded, expanded->prev = tail;
    
                while (expanded->next) expanded = expanded->next;
                tail = expanded;
            }
    
            // Add matched token
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
    
        // Raw segment after last match
        if (lastPos < pos + len) {
            auto raw = std::make_shared<LListNode<Token>>();
            raw->data = std::make_shared<Token>();
            raw->data->type = Type::RAW;
            raw->data->pos = lastPos;
            raw->data->len = static_cast<uint16_t>((pos + len) - lastPos);
    
            auto expanded = processToken(raw, type, src);
    
            tail->next = expanded;
            expanded->prev = tail;
    
            while (expanded->next) expanded = expanded->next;
            tail = expanded;
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
    
        for (int i = static_cast<int>(Type::MLINE_COMMENT); i <= static_cast<int>(Type::MULT); ++i) {
            Type type = static_cast<Type>(i);
            if (TOKEN_EXPR.find(type) == TOKEN_EXPR.end()) continue;
    
            std::shared_ptr<LListNode<Token>> curr = list.head;
            while (curr) {
                std::shared_ptr<LListNode<Token>> next = curr->next;
    
                if (curr->data->type == Type::RAW) {
                    std::shared_ptr<LListNode<Token>> processed = processToken(curr, type, src);
    
                    if (processed != curr) {
                        list.replace(curr, processed);
                        std::shared_ptr<LListNode<Token>> last = processed;
                        while (last->next) last = last->next;
                        curr = last->next;
                        continue;
                    }
                }
    
                curr = next;
            }
        }
    
        return std::make_shared<LList<Token>>(list);
    }
    

}  // namespace Blam
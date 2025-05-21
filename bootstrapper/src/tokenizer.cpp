#include <iostream>
#include "tokenizer.hpp"
#include "utils.hpp"
#include <regex>

namespace Tokenizer {

std::vector<Token> tokenizeRawNode(const std::string& input, Type type, uint16_t pos, uint16_t len) {
    std::vector<Token> tokens;

    std::string expression = tokenExpression[type];
    std::regex regexPattern(expression);
    auto words_begin = std::sregex_iterator(input.begin() + pos, input.begin() + pos + len, regexPattern);
    auto words_end = std::sregex_iterator();

    uint16_t lastPos = pos;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        
        // Add raw token for text between matches
        if (match.position() > lastPos) {
            Token rawToken;
            rawToken.type = Type::RAW;
            rawToken.pos = lastPos;
            rawToken.len = static_cast<uint16_t>(match.position() - lastPos);
            tokens.push_back(rawToken);
        }

        // Add matched token
        Token token;
        token.type = type;
        token.pos = static_cast<uint16_t>(match.position());
        token.len = static_cast<uint16_t>(match.length());
        tokens.push_back(token);

        lastPos = static_cast<uint16_t>(match.position() + match.length());
    }

    // Add raw token for text after the last match
    if (lastPos < pos + len) {
        Token rawToken;
        rawToken.type = Type::RAW;
        rawToken.pos = lastPos;
        rawToken.len = static_cast<uint16_t>((pos + len) - lastPos);
        tokens.push_back(rawToken);
    }
    
    return tokens;
}

std::shared_ptr<Token> insertToken(std::shared_ptr<Token> current, std::shared_ptr<Token> start) {
    auto end = start;
    while (end->next != nullptr) {
        end = end->next;
    }
    
    auto prev = current->prev;
    auto next = current->next;

    if (prev) {
        prev->next = start;
        start->prev = prev;
    } else {
        current = start; // Update current if it's the head
    }

    if (next) {
        next->prev = end;
        end->next = next;
    }

    return current;
}

std::shared_ptr<Token> processRawToken(std::shared_ptr<Token> token, Type type, const std::string& input) {
    // token info
    uint16_t pos = token->pos;
    uint16_t len = token->len;
    auto next = token->next;
    auto prev = token->prev;

    // locate regex matches
    std::regex reg(tokenExpression[type]);
    auto regStart = std::sregex_iterator(input.begin() + pos, input.begin() + pos + len, reg);
    auto regEnd = std::sregex_iterator();

    // keep track of last position of match
    uint16_t lastPos = pos;     

    int matchCount = std::distance(regStart, regEnd);
    std::cout << "Number of matches: " << matchCount << std::endl;

    for (auto it = regStart; it != regEnd; ++it) {
        const std::smatch& match = *it;

        // add raw token between tokens
        if (match.position() > lastPos) {
            auto rawToken = std::make_shared<Token>();
            rawToken->type = Type::RAW;
            rawToken->pos = lastPos;
            rawToken->len = static_cast<uint16_t>(match.position() - lastPos);
            std::cout << input.substr(lastPos, match.position() - lastPos) << std::endl;
            token = insertToken(token, rawToken);
        }

        // add matched token
        auto matchToken = std::make_shared<Token>();
        matchToken->type = type;
        matchToken->pos = static_cast<uint16_t>(match.position());
        matchToken->len = static_cast<uint16_t>(match.length());
        token = insertToken(token, matchToken);
        std::cout << "Matched token: " << input.substr(matchToken->pos, matchToken->len) << std::endl;

        lastPos = static_cast<uint16_t>(match.position() + match.length());
    }

    // add raw token for text after last match
    if (lastPos < pos + len) {
        auto rawToken = std::make_shared<Token>();
        rawToken->type = Type::RAW;
        rawToken->pos = lastPos;
        rawToken->len = static_cast<uint16_t>((pos + len) - lastPos);
        token = insertToken(token, rawToken);
    }

    return token;
}

std::shared_ptr<Token> tokenize(const std::string& input) {
    auto head = std::make_shared<Token>();
    head->next = nullptr;
    head->type = Type::RAW; // Initialize type
    head->pos = 0;
    head->len = static_cast<uint16_t>(input.length());
    
    // iteratively loop over token types
    for (int i = static_cast<int>(Type::RAW); i <= static_cast<int>(Type::KEYWORD); ++i) {
        Type type = static_cast<Type>(i);
        std::string expression = tokenExpression[type];

        // skip conditions
        if (type == Type::RAW || expression == "") {
            continue;
        }

        // iterate over tokens
        auto curr = head;
        while (curr != nullptr) {
            if (curr->type == Type::RAW) {
                auto next = curr->next;

                curr = processRawToken(curr, type, input);

                // Update head if the current token is the head and has been replaced
                if (curr == head && curr->type != Type::RAW) {
                    head = curr;
                }

                curr = next;
            } else {
                curr = curr->next;
            }
        }
    }

    // print tokens
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

    return head;
}

std::shared_ptr<Token> tokenizeFile(const std::string& fp) {
    const std::string contents = Blam::readFile(fp);
    return tokenize(contents);
}

}
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

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;

    // initial token
    Token token{};
    token.pos = 0;
    token.len = static_cast<uint16_t>(input.length());
    tokens.push_back(token);

    // iteratively loop over token types
    for (int i = static_cast<int>(Type::RAW); i <= static_cast<int>(Type::KEYWORD); ++i) {
        Type type = static_cast<Type>(i);

        // skip raw type
        if (type == Type::RAW) continue;

        // process lexeme
        std::string expression = tokenExpression[type];
        if (expression == "") continue;

        for (auto it = tokens.begin(); it != tokens.end(); ++it) {
            if (it->type != Type::RAW) continue;

            auto curr = *it;
            auto next = std::next(it);

            std::vector<Token> result = tokenizeRawNode(input, type, it->pos, it->len);
            it = tokens.erase(it);
            it = tokens.insert(it, result.begin(), result.end());
            std::advance(it, result.size() - 1);
        }
    }

    for (const auto& token : tokens) {
            std::cout << "Token Type: " << static_cast<int>(token.type)
                      << ", Position: " << token.pos
                      << ", Length: " << token.len
                      << ", Content: " << std::endl << input.substr(token.pos, token.len) << std::endl;
        }

    return tokens;
}

std::vector<Token> tokenizeFile(const std::string& fp) {
    const std::string contents = Blam::readFile(fp);
    return tokenize(contents);
}

}
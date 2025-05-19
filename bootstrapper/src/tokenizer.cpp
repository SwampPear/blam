#include "tokenizer.hpp"
#include "utils.hpp"

namespace Blam {

std::vector<Token> tokenize(const std::string& input) {
    /*
    std::vector<Token> tokens;
    size_t pos = 0;
    
    while (pos < input.length()) {
        if (std::isspace(input[pos])) {
            ++pos;
            continue;
        }
        
        size_t start = pos;
        while (pos < input.length() && !std::isspace(input[pos])) {
            ++pos;
        }
        
        Token token;
        token.src = input.c_str() + start;
        token.pos = static_cast<uint16_t>(start);
        token.len = static_cast<uint16_t>(pos - start);
        tokens.push_back(token);
    }

    return tokens;
    */
}

std::vector<Token> tokenizeFile(const std::string& fp) {
    const std::string contents = Blam::readFile(fp);
    return tokenize(contents);
}

}
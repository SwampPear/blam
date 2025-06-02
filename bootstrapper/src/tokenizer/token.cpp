#include <iostream>
#include <sstream>
#include <memory>

#include "tokenizer/token.hpp"

namespace Blam {

std::string Token::toString(const std::string& src) {
    std::ostringstream oss;
    oss << "Type: " << static_cast<int>(this->type)
        << ", Pos: " << this->pos
        << ", Len: " << this->len
        << ", Content: " << std::endl
        << src.substr(this->pos, this->len) << std::endl;

    return oss.str();
}

void Token::print(const std::string& src) {
    auto curr = std::make_shared<Token>(*this);
    while (curr != nullptr) {
        std::cout << curr->toString(src);
        curr = curr->next;
    }
}

}  // namespace Blam
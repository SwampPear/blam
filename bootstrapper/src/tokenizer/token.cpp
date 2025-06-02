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

}  // namespace Blam
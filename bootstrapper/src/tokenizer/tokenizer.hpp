#pragma once

#include <string>
#include <memory>

#include "core/llist.hpp"
#include "tokenizer/token.hpp"

namespace Blam {

//std::shared_ptr<Token> processRawToken(std::shared_ptr<Token> victim, Type type, const std::string& input);
std::shared_ptr<LListNode<Token>> processToken(std::shared_ptr<LListNode<Token>> token, Type type, const std::string& src);
std::shared_ptr<LList<Token>> tokenize(const std::string& src);

}  // namespace Blam
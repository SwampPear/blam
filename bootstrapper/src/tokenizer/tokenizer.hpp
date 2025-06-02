#pragma once

#include <string>
#include <vector>
#include <memory>

#include "tokenizer/token.hpp"

#include "core/utils.hpp"
#include "core/llist.hpp"

namespace Blam {

std::shared_ptr<Token> processRawToken(std::shared_ptr<Token> victim, Type type, const std::string& input);
std::unique_ptr<LList<Token>> tokenize(const std::string& src);

}  // namespace Blam
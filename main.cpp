#include <cstdlib>
#include <iostream>

#include "blam.hpp"

using namespace Blam;

int main() {
    std::string src = "a + b * (3 + 4)";
    auto tokens = Blam::tokenize(src);

    auto curr = tokens->head;
    while (curr) {  
        std::cout << (size_t)curr->data->type << " " << curr->data->pos << " " << curr->data->len << '\n';
        curr = curr->next;
    }

    Parser parser(tokens->head, src);
    auto ast = parser.parseExpression();
}
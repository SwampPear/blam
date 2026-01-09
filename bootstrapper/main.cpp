#include <cstdlib>

#include <vector>

#include "codegen/codegen.hpp"
#include "core/llist.hpp"
#include "parser/parser.hpp"
#include "tokenizer/token.hpp"
#include "tokenizer/tokenizer.hpp"

using namespace Blam;



int main() {
    std::string fp = "compiler/src/main.blam";
    std::string src = readFile(fp);

    log("File read.");

    std::shared_ptr<LList<Token>> tokens = tokenize(src);
    std::vector<Token> tokenVec;
    tokenVec.reserve(256);
    for (auto node = tokens->head; node; node = node->next) {
        if (node->data) {
            tokenVec.push_back(*node->data);
        }
    }

    std::unique_ptr<Stmt> program = parseProgram(tokenVec, src);

    generateIR(std::move(program), "output.ll");

    return EXIT_SUCCESS;
}

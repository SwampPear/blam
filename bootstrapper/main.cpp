#include <cstdlib>

#include <string>
#include <vector>

#include "codegen/codegen.hpp"
#include "core/llist.hpp"
#include "parser/parser.hpp"
#include "tokenizer/token.hpp"
#include "tokenizer/tokenizer.hpp"

using namespace Blam;
int main(int argc, char** argv) {
    std::string fp = "compiler/src/main.blam";
    std::string outputPath = "output.ll";
    std::string exePath = "blam_exec";
    std::string clangPath;
    if (argc > 1 && argv[1] && argv[1][0] != '\0') {
        fp = argv[1];
    }
    if (argc > 2 && argv[2] && argv[2][0] != '\0') {
        outputPath = argv[2];
    }
    if (argc > 3 && argv[3] && argv[3][0] != '\0') {
        exePath = argv[3];
    }
    if (argc > 4 && argv[4] && argv[4][0] != '\0') {
        clangPath = argv[4];
    }
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

    generateIR(std::move(program), outputPath);
    if (!compileIRToExecutable(outputPath, exePath, clangPath)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#pragma once

#include <string>
#include <sstream>

/**
 * Generates intermediate code (assembly) from abstract tokens.
 */
namespace BlamIRGenerator {
    std::string generateHeader() {
        std::stringstream ss;

        ss << ".section __TEXT,__text,regular,pure_instructions\n";
        ss << ".globl _start\n";
        ss << ".align 4\n";

        return ss.str();
    }

}  // namespace Blam
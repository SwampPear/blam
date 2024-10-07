#include <string>
#include <sstream>

/**
 * Generates intermediate representation code from ASTs.
 */
namespace BlamIRGenerator {

/**
 * Generates a header for this program.
 * TODO: should be replaced with LLVM IR
 */
std::string generate_header() {
    std::stringstream ss;

    ss << ".section __TEXT,__text,regular,pure_instructions\n";
    ss << ".globl _start\n";
    ss << ".align 4\n";

    return ss.str();
}

}  // namespace Blam
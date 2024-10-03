#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

/**
 * Intermediate compiler for intermediate representation code. This code exists
 * so that bootstrapper can be implemented independent of machine code
 * generation and linking.
 */
namespace BlamIRCompiler {

/**
 * Creates an executable from assembly code. Generates code with as and links
 * with ld.
 */
void executable() {
    const char* assemblyCommand = "as -o build/main.o build/main.s";
    const char* linkerCommand = "ld -o build/main build/main.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64";

    std::system(assemblyCommand);       // assemble
    std::system(linkerCommand);         // link

    // remove temporary files
    std::system("rm build/main.o");
}

}  // namespace Blam
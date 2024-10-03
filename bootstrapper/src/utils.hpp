#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Blam {

std::string readFile(const std::string& filePath) {
    std::ifstream fileStream(filePath);

    if (!fileStream.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;

        return std::string("");
    }

    std::stringstream buffer;

    buffer << fileStream.rdbuf();

    return std::string(buffer.str());
}

}  // Blam
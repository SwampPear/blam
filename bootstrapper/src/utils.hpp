#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

/**
 * Some useful functionality.
 */
namespace BlamUtils {

std::string read_file(const std::string& filePath) {
    std::ifstream fileStream(filePath);

    if (!fileStream.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;

        return std::string("");
    }

    std::stringstream buffer;

    buffer << fileStream.rdbuf();

    return std::string(buffer.str());
}

}  // namespace BlamUtils
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>

std::filesystem::path getCWD() {
    return std::filesystem::current_path();
}

std::string readFile(const std::string& filePath) {
    std::ifstream fileStream(filePath);

    if (!fileStream.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;

        return "";
    }

    std::stringstream buffer;

    buffer << fileStream.rdbuf();

    return buffer.str();
}

int main() {
    std::filesystem::path cwd = getCWD();
    std::string fileContents = readFile(cwd);

    std::cout << fileContents << std::endl;

    return EXIT_SUCCESS;
}

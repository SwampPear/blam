#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>

std::filesystem::path getCWD() {
    return std::filesystem::current_path();
}

std::shared_ptr<std::string> readFile(const std::string& filePath) {
    std::ifstream fileStream(filePath);

    if (!fileStream.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;

        return std::make_shared<std::string>("");
    }

    std::stringstream buffer;

    buffer << fileStream.rdbuf();

    return std::make_shared<std::string>(buffer.str());
}
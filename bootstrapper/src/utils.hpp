#pragma once

#include <fstream>
#include <string>
#include <stdexcept>

namespace Blam {


std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();
    return content;
}

std::string stripWhitespaceAndNewlines(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (!std::isspace(c) || c == ' ') {
            result += c;
        }
    }
    return result;
}



}  // namespace Blam
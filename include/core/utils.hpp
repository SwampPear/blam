#pragma once

#include <string>
#include <fstream>
#include <iostream>

namespace Blam {

inline std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::fprintf(stderr, "Could not open file: %s\n", filePath.c_str());
        std::abort();
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();
    return content;
}

inline void log(std::string msg) {
    std::cout << msg << std::endl;
}

}  // namespace Blam

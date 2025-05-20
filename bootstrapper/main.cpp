#include "src/tokenizer.hpp"

int main() {
    std::string fp = "example_project/src/main.blam";
    Tokenizer::tokenizeFile(fp);
    
    return 0;
}

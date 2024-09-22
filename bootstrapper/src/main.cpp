#include <iostream>
#include <cstdlib>
#include <filesystem>

std::filesystem::path get_cwd() {
    return std::filesystem::current_path();
}

int main() {
    std::cout << get_cwd() << std::endl;

    //get_cwd();  // Call the get_cwd function to print the current working directory

    return EXIT_SUCCESS;
}

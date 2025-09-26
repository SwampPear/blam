// src/main.cpp (demo: parse and print a simple success/fail)
#include <iostream>
#include <string>
#include "utils.hpp"
#include "parser.hpp"

using namespace blam;

int main(int argc, char **argv)
{
  try
  {
    std::string src = (argc > 1) ? blam::read_file(argv[1]) : R"(pub struct Point {
  x: int
  y: int
}

len(p: Point) -> int {
  if p.x > 0 {
    return p.x + p.y
  } else {
    return 0
  }
}
)";
    Parser p(src);
    auto mod = p.parse_module();
    std::cout << "Parsed module with " << mod->decls.size() << " top-level decl(s)\n";
    return 0;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Parse error: " << e.what() << "\n";
    return 1;
  }
}

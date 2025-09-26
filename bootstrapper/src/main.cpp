#include <iostream>
#include <filesystem>
#include <utils.hpp>
#include <lexer.hpp>

int main(int argc, char **argv)
{
  using namespace blam;
  std::filesystem::path path = (argc > 1) ? argv[1] : "-"; // "-" reads stdin
  std::string src = read_file(path);
  Lexer lx(src);
  for (;;)
  {
    Token t = lx.next();
    std::cout << to_string(t.kind) << " '" << t.lexeme << "'\n";
    if (t.kind == Tok::EOF_)
      break;
  }
}

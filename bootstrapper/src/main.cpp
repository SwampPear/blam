// main.cpp — minimal driver: read file, print tokens, parse
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "lexer.hpp"
#include "parser.hpp"
#include "tokens.hpp"
#include "ast.hpp"

using namespace blam;

static std::string slurp_file(const std::string &path)
{
  std::ifstream in(path, std::ios::binary);
  if (!in)
  {
    std::cerr << "error: cannot open " << path << "\n";
    std::exit(2);
  }
  std::ostringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

static std::string show_lexeme(const std::string &s)
{
  std::string out;
  out.reserve(s.size() + 2);
  out.push_back('"');
  for (char c : s)
  {
    if (c == '\n')
    {
      out += "\\n";
    }
    else if (c == '\r')
    {
      out += "\\r";
    }
    else if (c == '\t')
    {
      out += "\\t";
    }
    else if (c == '"')
    {
      out += "\\\"";
    }
    else
    {
      out.push_back(c);
    }
  }
  out.push_back('"');
  return out;
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    std::cerr << "Usage: blamc <file.blam>\n";
    return 2;
  }
  std::string src = slurp_file(argv[1]);

  try
  {
    // 1) Lex and print tokens
    Lexer lx(src);
    auto toks = lx.tokenize();
    for (const auto &t : toks)
    {
      std::cout << static_cast<int>(t.kind) << "\t" << show_lexeme(t.lexeme) << "\n";
    }

    // 2) Parse (construct a fresh parser from the source)
    Parser p(src);    // adjust if your Parser takes tokens/lexer
    p.parse_module(); // adjust to your actual entrypoint

    return 0;
  }
  catch (const LexError &e)
  {
    std::cerr << "Lex error: " << e.what() << "\n";
    return 3;
  }
  catch (const ParseError &e)
  {
    std::cerr << "Parse error: " << e.what() << "\n";
    return 4;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Unhandled error: " << e.what() << "\n";
    return 5;
  }
}

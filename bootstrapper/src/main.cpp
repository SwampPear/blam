// main.cpp — read file, lex to tokens, parse module, semantic analyze
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <type_traits>
#include <utility>

#include "lexer.hpp"
#include "parser.hpp"
#include "tokens.hpp"
#include "ast.hpp"
#include "sem.hpp" // <-- new

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

static bool env_debug_tokens()
{
  const char *v = std::getenv("BLAM_DEBUG_TOKENS");
  return v && *v && std::string(v) != "0";
}

static const char *safe_lexeme(const std::string &s)
{
  static thread_local std::string buf;
  constexpr size_t MAX_SHOW = 80;
  if (s.size() <= MAX_SHOW)
    return s.c_str();
  buf = s.substr(0, MAX_SHOW);
  buf += "...";
  return buf.c_str();
}

// ---- Compile-time detection helpers ----
template <typename T, typename = void>
struct has_tokenize : std::false_type
{
};
template <typename T>
struct has_tokenize<T, std::void_t<decltype(std::declval<T &>().tokenize())>> : std::true_type
{
};

template <typename T, typename = void>
struct has_next : std::false_type
{
};
template <typename T>
struct has_next<T, std::void_t<decltype(std::declval<T &>().next())>> : std::true_type
{
};

template <typename T, typename = void>
struct has_next_token : std::false_type
{
};
template <typename T>
struct has_next_token<T, std::void_t<decltype(std::declval<T &>().next_token())>> : std::true_type
{
};

template <typename...>
struct always_false : std::false_type
{
};

// Unified lex-all that works with multiple lexer styles.
template <typename L>
static std::vector<Token> lex_all(L &lx)
{
  if constexpr (has_tokenize<L>::value)
  {
    return lx.tokenize();
  }
  else if constexpr (has_next<L>::value)
  {
    std::vector<Token> out;
    for (;;)
    {
      Token t = lx.next();
      out.push_back(t);
      if (t.kind == Tok::EOF_)
        break;
    }
    return out;
  }
  else if constexpr (has_next_token<L>::value)
  {
    std::vector<Token> out;
    for (;;)
    {
      Token t = lx.next_token();
      out.push_back(t);
      if (t.kind == Tok::EOF_)
        break;
    }
    return out;
  }
  else
  {
    static_assert(always_false<L>::value, "Lexer must provide tokenize() or next()/next_token().");
  }
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    std::cerr << "Usage: blamc <file.blam>\n";
    return 2;
  }

  const std::string path = argv[1];
  const std::string src = slurp_file(path);

  try
  {
    // ---- LEX ----
    std::vector<Token> tokens;
    {
      Lexer lx(src);
      tokens = lex_all(lx);
    }

    if (tokens.empty() || tokens.back().kind != Tok::EOF_)
    {
      tokens.push_back(Token{Tok::EOF_, "", Range{}});
    }

    if (env_debug_tokens())
    {
      std::cerr << "== TOKENS ==\n";
      for (size_t i = 0; i < tokens.size(); ++i)
      {
        const Token &t = tokens[i];
        std::cerr << i << ": kind=" << static_cast<int>(t.kind)
                  << " lexeme=\"" << safe_lexeme(t.lexeme) << "\""
                  << " [start@" << t.range.start.line << ":" << t.range.start.col
                  << " end@" << t.range.end.line << ":" << t.range.end.col
                  << "]\n";
      }
      std::cerr << "=============\n";
    }

    // ---- PARSE ----
    Parser parser(std::move(tokens));
    std::shared_ptr<Module> mod = parser.parse_module();

    // ---- SEMANTIC ANALYSIS ----
    SemAnalyzer sem;
    sem.analyze(mod);

    std::cout << "OK: parsed module with " << mod->decls.size()
              << " top-level decl(s); semantic analysis passed\n";
    return 0;
  }
  catch (const ParseError &e)
  {
    std::cerr << "parse error: " << e.what() << "\n";
    return 1;
  }
  catch (const SemError &e)
  {
    const auto &w = e.where;
    std::cerr << "semantic error: line " << w.start.line << ", col " << w.start.col
              << ": " << e.what() << "\n";
    return 3;
  }
  catch (const std::exception &e)
  {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
}

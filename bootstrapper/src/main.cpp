// main.cpp — lex → parse → semantic analysis → (optional) LLVM IR emit
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"
#include "tokens.hpp"
#include "ast.hpp"
#include "sem.hpp"
#include "codegen_llvm.hpp"

using namespace blam;

// ---------- helpers ----------
static void print_usage()
{
  std::cerr << "Usage: blamc <file.blam> [--emit-ll out.ll] [--dump-tokens]\n";
}

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

// ---- compile-time detection helpers for different lexer APIs ----
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

// ---------- main ----------
int main(int argc, char **argv)
{
  // args: <file.blam> [--emit-ll out.ll] [--dump-tokens]
  std::string input;
  bool dumpTokens = env_debug_tokens();
  bool emitLL = false;
  std::string outLL;

  for (int i = 1; i < argc; ++i)
  {
    std::string a = argv[i];
    if (a == "--emit-ll")
    {
      if (i + 1 >= argc)
      {
        std::cerr << "error: --emit-ll needs a path\n";
        print_usage();
        return 2;
      }
      emitLL = true;
      outLL = argv[++i];
      continue;
    }
    if (a == "--dump-tokens")
    {
      dumpTokens = true;
      continue;
    }
    if (!a.empty() && a[0] == '-')
    {
      std::cerr << "error: unknown option: " << a << "\n";
      print_usage();
      return 2;
    }
    if (input.empty())
      input = a;
    else
    {
      std::cerr << "error: extra positional arg: " << a << "\n";
      print_usage();
      return 2;
    }
  }
  if (input.empty())
  {
    print_usage();
    return 2;
  }

  const std::string src = slurp_file(input);

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

    if (dumpTokens)
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
    Parser parser(std::move(tokens)); // your parser takes a token vector
    std::shared_ptr<Module> mod = parser.parse_module();

    // ---- SEMANTIC ANALYSIS ----
    SemAnalyzer sem;
    sem.analyze(mod);

    // ---- CODEGEN (optional LLVM IR emit) ----
    if (emitLL)
    {
      CodegenLLVM cg("blam");
      cg.emitModule(mod);
      if (!cg.writeToFile(outLL))
      {
        std::cerr << "error: failed to write IR to " << outLL << "\n";
        return 4;
      }
      std::cout << "wrote LLVM IR: " << outLL << "\n";
    }

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
    // e.where may be default if your AST doesn't carry ranges
    std::cerr << "semantic error";
    if (e.where.start.line)
    {
      std::cerr << ": line " << e.where.start.line << ", col " << e.where.start.col;
    }
    std::cerr << ": " << e.what() << "\n";
    return 3;
  }
  catch (const std::exception &e)
  {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
}

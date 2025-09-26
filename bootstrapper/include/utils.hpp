// include/utils.hpp
#pragma once
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace blam
{

  // Remove UTF-8 BOM if present
  inline void strip_utf8_bom(std::string &s)
  {
    if (s.size() >= 3 &&
        static_cast<unsigned char>(s[0]) == 0xEF &&
        static_cast<unsigned char>(s[1]) == 0xBB &&
        static_cast<unsigned char>(s[2]) == 0xBF)
    {
      s.erase(0, 3);
    }
  }

  // Read entire file to a std::string (binary-safe). Throws on error.
  // Pass "-" to read from stdin.
  inline std::string read_file(const std::filesystem::path &path)
  {
    if (path == "-")
    {
      // read stdin
      std::string data;
      std::vector<char> buf(8192);
      while (true)
      {
        std::streamsize n = std::cin.read(buf.data(), buf.size()).gcount();
        if (n > 0)
          data.append(buf.data(), static_cast<size_t>(n));
        if (!std::cin)
          break;
      }
      strip_utf8_bom(data);
      return data;
    }

    std::ifstream f(path, std::ios::in | std::ios::binary);
    if (!f)
      throw std::runtime_error("Failed to open file: " + path.string());

    f.seekg(0, std::ios::end);
    std::streampos end = f.tellg();
    if (end < 0)
      throw std::runtime_error("Failed to stat file: " + path.string());
    std::string data(static_cast<size_t>(end), '\0');
    f.seekg(0, std::ios::beg);
    f.read(data.data(), data.size());
    if (!f && !f.eof())
      throw std::runtime_error("Failed to read file: " + path.string());

    strip_utf8_bom(data);
    return data;
  }

  // Convenience overload
  inline std::string read_file(std::string_view p)
  {
    return read_file(std::filesystem::path(p));
  }

} // namespace blam

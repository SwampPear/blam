#pragma once

#include <memory>
#include <string>
#include "ast.hpp"

namespace blam
{

  class CodegenLLVM
  {
  public:
    explicit CodegenLLVM(std::string moduleName);
    ~CodegenLLVM();
    CodegenLLVM(CodegenLLVM &&) noexcept = default;
    CodegenLLVM &operator=(CodegenLLVM &&) noexcept = default;
    CodegenLLVM(const CodegenLLVM &) = delete;
    CodegenLLVM &operator=(const CodegenLLVM &) = delete;

    // Produce IR for the module (currently: function prototypes + trivial bodies).
    void emitModule(const std::shared_ptr<Module> &mod);

    // Dump IR to string or file.
    std::string str() const;
    bool writeToFile(const std::string &path) const;

  private:
    struct Impl;
    Impl *impl_; // PIMPL so we can stub when LLVM isn't available
  };

} // namespace blam

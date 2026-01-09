#pragma once

#include <memory>
#include <string>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include "parser/parser.hpp"

namespace Blam {

void generateIR(std::unique_ptr<Stmt> program, const std::string& outputPath);
bool compileIRToExecutable(const std::string& irPath, const std::string& exePath, const std::string& clangPath);

}  // namespace Blam

#pragma once

#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include "parser/parser.hpp"


namespace BlamBootstrapper {

void generateIR(std::unique_ptr<Stmt> program);

}  // namespace BlamBootstrapper
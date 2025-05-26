#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "parser/ast.hpp"

namespace BlamBootstrapper {

void generateIR(std::unique_ptr<Parser::Expr>& ast, llvm::LLVMContext& ctx, llvm::Module& module);

}  // BlamBootstrapper

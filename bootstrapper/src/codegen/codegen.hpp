#pragma once
#include "parser/ast.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace CodeGen {

void generateIR(std::unique_ptr<Parser::Expr>& ast, llvm::LLVMContext& ctx, llvm::Module& module);

}  // namespace CodeGen

#include "parser/ast.hpp"

namespace BlamBootstrapper {

llvm::Value* NumberExpr::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) {
    return llvm::ConstantFP::get(ctx, llvm::APFloat(value));
}

}  // namespace BlamBoostrapper
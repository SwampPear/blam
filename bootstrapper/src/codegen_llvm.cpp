#include "codegen_llvm.hpp"
#include <memory>
#include <vector>
#include <string>

#include <fstream>
#include <sstream>
#include <utility>

// LLVM headers
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

namespace blam
{

  struct CodegenLLVM::Impl
  {
    std::unique_ptr<llvm::LLVMContext> ctx;
    std::unique_ptr<llvm::Module> mod;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    explicit Impl(std::string moduleName)
    {
      ctx = std::make_unique<llvm::LLVMContext>();
      mod = std::make_unique<llvm::Module>(moduleName, *ctx);
      builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
    }

    llvm::Type *i32() { return llvm::Type::getInt32Ty(*ctx); }
    llvm::Type *i1() { return llvm::Type::getInt1Ty(*ctx); }
    llvm::Type *voidT() { return llvm::Type::getVoidTy(*ctx); }

    // Map Blam param count to dummy i32 params (until real typing is wired)
    std::vector<llvm::Type *> dummyParamTypes(size_t n)
    {
      return std::vector<llvm::Type *>(n, i32());
    }

    // Create (or get) a function with trivial body.
    void emitFunction(FuncDecl *f)
    {
      // For now: return type = void if no explicit return; else i32
      bool returnsVoid = true;
      if constexpr (requires { f->ret; })
      {
        if (f->ret.has_value())
          returnsVoid = false;
      }

      auto *retTy = returnsVoid ? voidT() : i32();
      auto paramTys = dummyParamTypes(f->params.size());
      auto *fnTy = llvm::FunctionType::get(retTy, paramTys, /*isVarArg*/ false);

      auto *fn = llvm::Function::Create(fnTy, llvm::Function::ExternalLinkage, f->name, mod.get());

      // Name parameters
      size_t idx = 0;
      for (auto &arg : fn->args())
      {
        arg.setName(f->params[idx].name);
        ++idx;
      }

      // Minimal body: entry + ret
      auto *entry = llvm::BasicBlock::Create(*ctx, "entry", fn);
      builder->SetInsertPoint(entry);
      if (returnsVoid)
      {
        builder->CreateRetVoid();
      }
      else
      {
        builder->CreateRet(llvm::ConstantInt::get(i32(), 0));
      }

      // Verify for sanity in debug builds
#ifndef NDEBUG
      if (llvm::verifyFunction(*fn, &llvm::errs()))
      {
        fn->print(llvm::errs());
        llvm::errs() << "\n";
      }
#endif
    }
  };

  CodegenLLVM::CodegenLLVM(std::string moduleName) : impl_(new Impl(std::move(moduleName))) {}
  CodegenLLVM::~CodegenLLVM() { delete impl_; }

  void CodegenLLVM::emitModule(const std::shared_ptr<Module> &modAst)
  {
    if (!modAst)
      return;
    // Emit trivial functions for each top-level FuncDecl
    for (auto &d : modAst->decls)
    {
      if (auto *f = dynamic_cast<FuncDecl *>(d.get()))
      {
        impl_->emitFunction(f);
      }
    }
  }

  std::string CodegenLLVM::str() const
  {
    std::string s;
    llvm::raw_string_ostream os(s);
    impl_->mod->print(os, nullptr);
    return os.str();
  }

  bool CodegenLLVM::writeToFile(const std::string &path) const
  {
    std::ofstream out(path, std::ios::binary);
    if (!out)
      return false;
    std::string s;
    llvm::raw_string_ostream os(s);
    impl_->mod->print(os, nullptr);
    out << os.str();
    return true;
  }

} // namespace blam

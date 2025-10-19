/*
#include "blam/codegen_llvm.hpp"

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
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/Support/raw_ostream.h>
*/

/*
namespace blam
{

  struct CodegenLLVM::Impl
  {
    std::unique_ptr<llvm::LLVMContext> ctx;
    std::unique_ptr<llvm::Module> mod;
    std::unique_ptr<llvm::IRBuilder<>> builder;

    // cached externs
    llvm::Function *putsFn{nullptr};

    explicit Impl(std::string moduleName)
    {
      ctx = std::make_unique<llvm::LLVMContext>();
      mod = std::make_unique<llvm::Module>(moduleName, *ctx);
      builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
    }

    // --- common types ---
    llvm::Type *i8() { return llvm::Type::getInt8Ty(*ctx); }
    llvm::Type *i32() { return llvm::Type::getInt32Ty(*ctx); }
    llvm::Type *i1() { return llvm::Type::getInt1Ty(*ctx); }
    llvm::Type *voidT() { return llvm::Type::getVoidTy(*ctx); }
    llvm::PointerType *i8ptr() { return llvm::PointerType::getUnqual(i8()); }

    // Map Blam param count to dummy i32 params (placeholder until real typing)
    std::vector<llvm::Type *> dummyParamTypes(size_t n) { return std::vector<llvm::Type *>(n, i32()); }

    // --- externs ---
    llvm::Function *getPuts()
    {
      if (putsFn)
        return putsFn;
      auto *fty = llvm::FunctionType::get(i32(), {i8ptr()}, isVarArg=false);
      putsFn = llvm::Function::Create(fty, llvm::Function::ExternalLinkage, "puts", mod.get());
      return putsFn;
    }

    // --- literals ---
    // Lower a C-string (with terminating NUL) to a private global and return i8* pointer to its first element
    llvm::Value *emitCStringGlobal(const std::string &s, const llvm::Twine &symBase = ".str")
    {
      // Ensure it ends with '\n' if you want print to add newline; keep as-is otherwise.
      // Here we keep exactly what caller passes.
      auto &C = *ctx;
      auto data = llvm::ConstantDataArray::getString(C, s, /*AddNull=true);
      auto *arrTy = data->getType(); // [N x i8]

      auto *gv = new llvm::GlobalVariable(
          *mod, arrTy,
          /*isConstant=true,
          llvm::GlobalValue::PrivateLinkage,
          data, symBase);
      gv->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
      gv->setAlignment(llvm::MaybeAlign(1));

      llvm::Value *zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(C), 0);
      llvm::Value *idxs[] = {zero, zero};
      return builder->CreateInBoundsGEP(arrTy, gv, idxs, symBase + ".gep"); // i8*
    }

    // --- expr/stmt emit (very minimal for now) ---
    llvm::Value *emitExpr(const ExprPtr &e)
    {
      if (!e)
        return nullptr;

      // IdentExpr (only used for callee name in this minimal pass)
      if (auto *id = dynamic_cast<IdentExpr *>(e.get()))
      {
        // In a real compiler you'd resolve identifiers to functions/vars.
        // Here, we only support being used as a callee name ("print").
        // Return a sentinel null; CallExpr will re-inspect the node.
        return nullptr;
      }

      // String literal
      if (auto *se = dynamic_cast<StrExpr *>(e.get()))
      {
        // For a nicer UX, if user writes print("Hello world!"), add newline here.
        // You can choose policy; we'll append '\n' so puts prints nicely.
        std::string withNl = se->value;
        if (withNl.empty() || withNl.back() != '\n')
          withNl.push_back('\n');
        return emitCStringGlobal(withNl, ".str.hello");
      }

      // Call expression — only builtin "print" for now
      if (auto *call = dynamic_cast<CallExpr *>(e.get()))
      {
        // Expect callee to be an IdentExpr
        if (auto *ident = dynamic_cast<IdentExpr *>(call->callee.get()))
        {
          if (ident->value == "print")
          {
            // Expect one argument convertible to i8*
            if (call->args.empty())
            {
              // No args: print newline
              auto *arg = emitCStringGlobal("\n", ".str.nl");
              return builder->CreateCall(getPuts(), {arg});
            }

            llvm::Value *arg0 = emitExpr(call->args[0]);
            // If arg0 is not an i8*, but e.g. we add more types later, you'd convert it here.
            if (!arg0 || !arg0->getType()->isPointerTy())
            {
              // Fallback: force string literal path
              arg0 = emitCStringGlobal("<print unsupported type>\n", ".str.err");
            }
            return builder->CreateCall(getPuts(), {arg0});
          }
        }

        // Future: general function calls
        return nullptr;
      }

      // Other expressions not yet supported
      return nullptr;
    }

    void emitStmt(const StmtPtr &s)
    {
      if (!s)
        return;

      // Expression statement: evaluate for side effects
      if (auto *es = dynamic_cast<ExprStmt *>(s.get()))
      {
        (void)emitExpr(es->expr);
        return;
      }

      // Block
      if (auto *blk = dynamic_cast<BlockStmt *>(s.get()))
      {
        emitBlock(blk);
        return;
      }

      // Return (minimal)
      if (auto *rs = dynamic_cast<ReturnStmt *>(s.get()))
      {
        // Your AST should use optional<ExprPtr> value; adjust if different
        llvm::Function *fn = builder->GetInsertBlock()->getParent();
        if (fn->getReturnType()->isVoidTy())
        {
          builder->CreateRetVoid();
        }
        else
        {
          llvm::Value *v = nullptr;
          if (rs->value)
            v = emitExpr(rs->value);
          if (!v)
            v = llvm::ConstantInt::get(i32(), 0);
          builder->CreateRet(v);
        }
        return;
      }

      // (Add more stmt kinds as you implement them)
    }

    void emitBlock(BlockStmt *blk)
    {
      for (auto &st : blk->stmts)
      {
        // Stop if block already terminated (e.g., after a return)
        if (builder->GetInsertBlock()->getTerminator())
          break;
        emitStmt(st);
      }
    }

    static bool isTerminated(llvm::BasicBlock *bb)
    {
      return bb && bb->getTerminator() != nullptr;
    }

    void emitFunction(FuncDecl *f)
    {
      // Return type: void if no explicit return; else i32 as placeholder
      bool returnsVoid = !f->ret.has_value();
      auto *retTy = returnsVoid ? voidT() : i32();
      auto paramTys = dummyParamTypes(f->params.size());
      auto *fnTy = llvm::FunctionType::get(retTy, paramTys, /*isVarArg false);

      auto *fn = llvm::Function::Create(fnTy, llvm::Function::ExternalLinkage, f->name, mod.get());

      // Name parameters (placeholders)
      size_t idx = 0;
      for (auto &arg : fn->args())
        arg.setName(f->params[idx++].name);

      // Entry block
      auto *entry = llvm::BasicBlock::Create(*ctx, "entry", fn);
      builder->SetInsertPoint(entry);

      // Emit body if present, else trivial return
      if (f->body)
      {
        emitBlock(f->body.get());
      }

      // Ensure function is terminated
      if (!isTerminated(builder->GetInsertBlock()))
      {
        if (returnsVoid)
          builder->CreateRetVoid();
        else
          builder->CreateRet(llvm::ConstantInt::get(i32(), 0));
      }

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
    for (auto &d : modAst->decls)
      if (auto *f = dynamic_cast<FuncDecl *>(d.get()))
        impl_->emitFunction(f);
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
*/
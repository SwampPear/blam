#include "parser/ast.hpp"

namespace BlamBootstrapper {

llvm::Value* NumberExpr::codegen(llvm::LLVMContext& ctx, llvm::IRBuilder<>& builder) {
    return llvm::ConstantFP::get(ctx, llvm::APFloat(value));
}

void printAST(const std::unique_ptr<Stmt>& stmt, int indent = 0) {
    if (!stmt) return;

    std::string indentation(indent, ' ');

    switch (stmt->stmtType) {
        case StmtType::SCOPED: {
            auto pStmt = static_cast<ScopedStmt*>(stmt.get());
            std::cout << indentation << "ScopedStmt: " << std::endl;

            for (const auto& stmt : pStmt->body) {
                printAST(stmt, indent + 2);
            }

            break;
        }
        case StmtType::PUB: {
            auto pStmt = static_cast<PubStmt*>(stmt.get());
            std::cout << indentation << "PubStmt: " << std::endl;
            printAST(pStmt->body, indent + 2);

            break;
        }
        case StmtType::FUNC_DECL: {
            auto pStmt = static_cast<FuncDeclStmt*>(stmt.get());
            std::cout << indentation << "FuncDecl(" << pStmt->name << "): " << std::endl;

            for (const auto& stmt : pStmt->body->body) {
                printAST(stmt, indent + 2);
            }
            
            break;
        }
        case StmtType::VAR_DECL: {
            auto pStmt = static_cast<VarDeclStmt*>(stmt.get());
            std::cout << indentation << "VarDecl(" << pStmt->name << ")" << std::endl;
            break;
        }

        case StmtType::RETURN: {
            auto pStmt = static_cast<ReturnStmt*>(stmt.get());
            std::cout << indentation << "Return" << std::endl;
            break;
        }
        default:
            std::cout << indentation << "Unknown StmtType" << std::endl;
            break;
    }
}

}  // namespace BlamBoostrapper
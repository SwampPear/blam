#include "ast.hpp"
#include "tokenizer/tokenizer.hpp"

namespace Blam {

class Parser {
public:
    explicit Parser(std::shared_ptr<LListNode<Token>> tokens);

    std::shared_ptr<Expr> parseExpression();
    std::shared_ptr<Expr> parsePrimary();
    std::shared_ptr<Expr> parseBinaryOpRHS(int prec, std::shared_ptr<Expr> lhs);

private:
    std::shared_ptr<LListNode<Token>> current;

    void advance();
    Token& peek();
    bool match(Type type);
    int getPrecedence(Type type);
};

}  // namespace Blam

#include "ast/ast.hpp"
#include "tokenizer/tokenizer.hpp"

namespace Blam {

class Parser {
public:
    explicit Parser(std::shared_ptr<LListNode<Token>> tokens, const std::string& src);

    std::shared_ptr<Expr> parseExpression();
    std::shared_ptr<Expr> parsePrimary();
    std::shared_ptr<Expr> parseBinaryOpRHS(int prec, std::shared_ptr<Expr> lhs);
    std::vector<std::shared_ptr<Expr>> parse();  // top-level parser

private:
    std::shared_ptr<LListNode<Token>> curr;
    const std::string& src;

    void advance();
    Token& peek();
    bool match(Type type);
    int getPrecedence(Type type);
    void skipWhitespace();
    bool isRightAssociative(Type op);
};

}  // namespace Blam

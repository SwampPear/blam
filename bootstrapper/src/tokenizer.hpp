namespace BlamTokenizer {
struct Token {
    int tokenType;
    int start;
    int end;
};

struct Lexeme {
    std::string name;   // name of Lexeme (i.e. "STRING")
    std::string expr;   // expression to match (i.e. "\"[a-zA-Z0-9\\s\\}]*\"")
};

struct LLNode {
    std::shared_ptr<TokenData> data;
    std::shared_ptr<LLNode> next;
    std::shared_ptr<LLNode> prev;
};

}  // namespace BlamTokenizer
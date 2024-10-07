namespace BlamAnalyzer {
/**
 * A token for simultaneous use in tokenization and abstract syntax tree 
 * parsing.
 */
struct Token {
    int type;           // enum type of node
    int start;          // start index in source
    int end;            // end index in source

    Token *next;        // next node in linked list
    Token *prev;        // prev node in linked list
    Token *first;        // first child node
    Token *last;        // last child node
};

/**
 * Replaces a range of tokens with a replacement token.
 * @param first - the first token in the range to replace
 * @param last - the last token in the range to replace
 * @param replacement - the token to replace the range with
 */
void replace_range(Token* first, Token* last, Token* replacement) {
    
}


struct Lexeme {
    std::string name;   // name of Lexeme (i.e. "STRING")
    std::string expr;   // expression to match (i.e. "\"[a-zA-Z0-9\\s\\}]*\"")
};

}  // namespace BlamTokenizer
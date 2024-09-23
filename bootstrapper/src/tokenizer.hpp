#include <sstream>
#include <memory>
#include <map>

// lexeme types of token
enum TOKEN_TYPE {
    STRING,
    CONTENT,
    SINGLE_LINE_COMMENT,
    MULTI_LINE_COMMENT,
    L_DELIMETER,
    R_DELIMETER,
    L_CURLY_DELIMETER,
    R_CURLY_DELIMETER,
    L_SQUARE_DELIMETER,
    R_SQUARE_DELIMETER,
    OP_DOT,
    OP_PLUS,
    OP_MINUS,
    OP_EQUALS,
    OP_SLASH,
    SPACE,
};

// maps pertinent token type to corresponding regular expression
const std::map<TOKEN_TYPE, std::string> tokenExpressions {
    {TOKEN_TYPE::STRING, "\"[a-zA-Z0-9\/\\\s]*\""},
    {TOKEN_TYPE::SINGLE_LINE_COMMENT, "\/\/[\sa-zA-Z0-9]*\n*"},
    {TOKEN_TYPE::MULTI_LINE_COMMENT, "\/\*[\sa-zA-Z0-9]*\*\/"},
    {TOKEN_TYPE::L_DELIMETER, "\("},
    {TOKEN_TYPE::R_DELIMETER, "\)"},
    {TOKEN_TYPE::L_CURLY_DELIMETER, "{"},
    {TOKEN_TYPE::R_CURLY_DELIMETER, "}"},
    {TOKEN_TYPE::L_SQUARE_DELIMETER, "\["},
    {TOKEN_TYPE::R_SQUARE_DELIMETER, "\]"},
    {TOKEN_TYPE::OP_DOT, "."},
    {TOKEN_TYPE::OP_PLUS, "\+"},
    {TOKEN_TYPE::OP_MINUS, "-"},
    {TOKEN_TYPE::OP_EQUALS, "="},
    {TOKEN_TYPE::OP_SLASH, "/"},
    {TOKEN_TYPE::SPACE, "\s*"}
};

// maps token type to corresponding token name
const std::map<TOKEN_TYPE, std::string> tokenNames {
    {TOKEN_TYPE::CONTENT, "CONTENT"},
    {TOKEN_TYPE::STRING, "STRING"},
    {TOKEN_TYPE::SINGLE_LINE_COMMENT, "SINGLE_LINE_COMMENT"},
    {TOKEN_TYPE::MULTI_LINE_COMMENT, "MULTI_LINE_COMMENT"},
    {TOKEN_TYPE::L_DELIMETER, "L_DELIMETER"},
    {TOKEN_TYPE::R_DELIMETER, "R_DELIMETER"},
    {TOKEN_TYPE::L_CURLY_DELIMETER, "L_CURLY_DELIMETER"},
    {TOKEN_TYPE::R_CURLY_DELIMETER, "R_CURLY_DELIMETER"},
    {TOKEN_TYPE::L_SQUARE_DELIMETER, "L_SQUARE_DELIMETER"},
    {TOKEN_TYPE::R_SQUARE_DELIMETER, "L_SQUARE_DELIMETER"},
    {TOKEN_TYPE::OP_DOT, "OP_DOT"},
    {TOKEN_TYPE::OP_PLUS, "OP_PLUS"},
    {TOKEN_TYPE::OP_MINUS, "OP_MINUS"},
    {TOKEN_TYPE::OP_EQUALS, "OP_EQUALS"},
    {TOKEN_TYPE::OP_SLASH, "OP_SLASH"},
    {TOKEN_TYPE::SPACE, "SPACE"}
};

struct TokenizerNode {
    TOKEN_TYPE tokenType;
    std::shared_ptr<std::string> src;
    int start;
    int end;
    std::shared_ptr<TokenizerNode> next;
};

std::string tokenizerNodeToString(TokenizerNode &tokenNode) {
    std::stringstream ss;
    ss << "<" << tokenNames.at(tokenNode.tokenType) << " ";
    ss << tokenNode.start << " ";
    ss << tokenNode.end << ">\n";
    ss << *tokenNode.src << std::endl;

    std::string output = ss.str();

    return output;
}

void tokenizeNode(TokenizerNode &root, std::string lexeme) {

}

int tokenize(std::shared_ptr<std::string> srcContents) {
    // init root content node
    std::shared_ptr<TokenizerNode> root = std::make_shared<TokenizerNode>();
    root->tokenType = TOKEN_TYPE::CONTENT;
    root->src = srcContents;
    root->start = 0;
    root->end = srcContents.get()->length();
    root->next = nullptr;

    std::cout << tokenizerNodeToString(*root) << std::endl;

    return 0;
}

#include <sstream>
#include <memory>
#include <map>
#include <regex>

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
    KEYWORD
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
    {TOKEN_TYPE::SPACE, "\s*"},
    {TOKEN_TYPE::KEYWORD, "return"}
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
    {TOKEN_TYPE::SPACE, "SPACE"},
    {TOKEN_TYPE::KEYWORD, "KEYWORD"}
};

struct TokenizerNode {
    TOKEN_TYPE tokenType;
    std::shared_ptr<std::string> src;
    int start;
    int end;
    std::shared_ptr<TokenizerNode> prev;
    std::shared_ptr<TokenizerNode> next;
};

std::string tokenizerNodeToString(std::shared_ptr<TokenizerNode> tokenNode) {
    std::stringstream ss;

    while(tokenNode != nullptr) {
        ss << "<" << tokenNames.at(tokenNode->tokenType) << " ";
        ss << tokenNode->start << " ";
        ss << tokenNode->end << ">\n";

        std::string src = *tokenNode->src;
        std::string srcInd = src.substr(tokenNode->start, tokenNode->end - tokenNode->start);

        ss << srcInd << std::endl;

        tokenNode = tokenNode->next;
    }

    std::string output = ss.str();

    return output;
}

void tokenizeContentNode(std::shared_ptr<TokenizerNode> node, TOKEN_TYPE tokenType) {
    // lexical expression for token of tokenType
    std::string lexeme = tokenExpressions.at(tokenType);

    // record old state
    std::shared_ptr<TokenizerNode> oldPrev = node->prev;
    std::shared_ptr<TokenizerNode> oldNext = node->next;
    int oldStart = node->start;
    int oldEnd = node->end;

    // make new root node for within content node scope
    std::shared_ptr<TokenizerNode> newRoot = nullptr;

    // record current node
    std::shared_ptr<TokenizerNode> currNode = node;

    // src file content (should be same for all nodes)
    std::string src = *node->src;

    // src file content for within content node scope
    std::string input = src.substr(node->start, node->end - node->start);

    std::regex regex(lexeme);   // digested regex pattern
    std::smatch match;          // match results

    // search for all matches
    auto begin = std::sregex_iterator(input.begin(), input.end(), regex);
    auto end = std::sregex_iterator();

    // current index of search
    int currIndex = 0;
    int matchStart = 0;
    int matchEnd = 0;

    for (auto it = begin; it != end; ++it) {
        match = *it;

        // extract match information
        std::string matchContents = match.str(0);
        matchStart = match.position(0);
        matchEnd = matchStart + match.length(0);

        // create and insert new content node
        if (currIndex != matchStart) {
            std::shared_ptr<TokenizerNode> newNode = std::make_shared<TokenizerNode>();

            newNode->tokenType = TOKEN_TYPE::CONTENT;
            newNode->src = node->src;
            newNode->start = currIndex;
            newNode->end = matchStart;
            newNode->prev = nullptr;
            newNode->next = nullptr;

            // iterate current node
            currNode = newNode;
            currIndex = matchEnd;

            // update new root if not set
            if (newRoot == nullptr) {
                newRoot = newNode;
            }
        }

        // create and insert new lexical node
        std::shared_ptr<TokenizerNode> newNode = std::make_shared<TokenizerNode>();
        newNode->tokenType = tokenType;
        newNode->src = node->src;
        newNode->start = matchStart;
        newNode->end = matchEnd;
        newNode->prev = currNode;
        newNode->next = nullptr;

        // iterate current node
        currNode->next = newNode;
        currNode = newNode;
        currIndex = matchEnd;

        // update new root if not set
        if (newRoot == nullptr) {
            newRoot = newNode;
        }

        // connect new node previous
        if (oldPrev == nullptr) {
            node = newRoot;
        } else {
            newRoot->next = oldNext;
            oldPrev->next = newRoot;
        }
    }

    if (currIndex != oldEnd) {
        std::cout << "adsf" << std::endl;
        // create and insert post content node
        std::shared_ptr<TokenizerNode> newNode = std::make_shared<TokenizerNode>();
        newNode->tokenType = TOKEN_TYPE::CONTENT;
        newNode->src = node->src;
        newNode->start = currIndex;
        newNode->end = oldEnd;
        newNode->prev = currNode;
        newNode->next = oldNext;

        // iterate current node
        currNode->next = newNode;
        currNode = newNode;
        currIndex = matchEnd;
    }

    std::cout << tokenizerNodeToString(node) << std::endl;
}

int tokenize(std::shared_ptr<std::string> srcContents) {
    // init root content node
    std::shared_ptr<TokenizerNode> root = std::make_shared<TokenizerNode>();
    root->tokenType = TOKEN_TYPE::CONTENT;
    root->src = srcContents;
    root->start = 0;
    root->end = srcContents.get()->length();
    root->prev = nullptr;
    root->next = nullptr;

    // tokenize for each lexeme
    tokenizeContentNode(root, TOKEN_TYPE::STRING);

    return 0;
}

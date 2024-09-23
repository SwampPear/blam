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
    {TOKEN_TYPE::STRING, "\"[a-zA-Z0-9\\s]*\""},
    {TOKEN_TYPE::SINGLE_LINE_COMMENT, "\\/\\/[\\sa-zA-Z0-9]*\n*"},
    {TOKEN_TYPE::MULTI_LINE_COMMENT, "\\/\\*[\\sa-zA-Z0-9]*\\*\\/"},
    {TOKEN_TYPE::L_DELIMETER, "\\("},
    {TOKEN_TYPE::R_DELIMETER, "\\)"},
    {TOKEN_TYPE::L_CURLY_DELIMETER, "\\{"},
    {TOKEN_TYPE::R_CURLY_DELIMETER, "\\}"},
    {TOKEN_TYPE::L_SQUARE_DELIMETER, "\\["},
    {TOKEN_TYPE::R_SQUARE_DELIMETER, "\\]"},
    {TOKEN_TYPE::OP_DOT, "."},
    {TOKEN_TYPE::OP_PLUS, "\\+"},
    {TOKEN_TYPE::OP_MINUS, "-"},
    {TOKEN_TYPE::OP_EQUALS, "="},
    {TOKEN_TYPE::OP_SLASH, "/"},
    {TOKEN_TYPE::SPACE, "\\s*"},
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
    std::string *src;
    int start;
    int end;
    TokenizerNode *prev;
    TokenizerNode *next;
};

std::string tokenizerNodeToString(TokenizerNode *tokenNode) {
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

void tokenizeContentNode(TokenizerNode *node, TOKEN_TYPE tokenType) {
    std::string lexeme = tokenExpressions.at(tokenType);

    // old state
    TokenizerNode *oldPrev = node->prev;
    TokenizerNode *oldNext = node->next;
    int oldEnd = node->end;
    std::string *src = node->src;           // will be copied to all nodes

    TokenizerNode *newRoot = nullptr;
    TokenizerNode *currNode = node;

    // src file content for within content node scope
    std::string input = *src;
    input = input.substr(node->start, node->end - node->start);

    // match lexeme
    std::regex regex(lexeme);
    std::smatch match;
    auto begin = std::sregex_iterator(input.begin(), input.end(), regex);
    auto end = std::sregex_iterator();

    // current index of search
    int currIndex = 0;
    int matchStart = 0;
    int matchEnd = 0;
    bool matchFound = false;

    for (auto it = begin; it != end; ++it) {
        matchFound = true;

        match = *it;

        // match info
        std::string matchContents = match.str(0);
        matchStart = match.position(0);
        matchEnd = matchStart + match.length(0);

        // create and insert new content node if needed
        if (currIndex != matchStart) {
            // create node
            TokenizerNode *newNode = new TokenizerNode;
            newNode->tokenType = TOKEN_TYPE::CONTENT;
            newNode->src = src;
            newNode->start = currIndex;
            newNode->end = matchStart;

            // update position
            newNode->prev = currNode;
            newNode->next = nullptr;

            // update current node
            currNode = newNode;

            // set new root if not set
            if (newRoot == nullptr) {
                newRoot = currNode;
            }

            // update index
            currIndex = matchStart;

            // update position relative to node before
            if (currNode->prev != nullptr) {
                currNode->prev->next = currNode;
            }
        }

        // create and insert new lexical node
        TokenizerNode *newNode = new TokenizerNode;
        newNode->tokenType = tokenType;
        newNode->src = src;
        newNode->start = matchStart;
        newNode->end = matchEnd;
        
        // update position
        newNode->prev = currNode;
        newNode->next = nullptr;

        // update current node
        currNode = newNode;

        // set new root if not set
        if (newRoot == nullptr) {
            newRoot = currNode;
        }

        // update index
        currIndex = matchEnd;

        // update position relative to node before
        if (currNode->prev != nullptr) {
            currNode->prev->next = currNode;
        }
    }

    // create and insert new content node at end
    if (currIndex != oldEnd && matchFound) {
        std::cout << "asdfasdf" << std::endl;
        // create and insert post content node
        TokenizerNode *newNode = new TokenizerNode;
        newNode->tokenType = TOKEN_TYPE::CONTENT;
        newNode->src = node->src;
        newNode->start = currIndex;
        newNode->end = oldEnd;

        // update position
        newNode->prev = currNode;
        newNode->next = nullptr;

        // update current node
        currNode = newNode;

        // set new root if not set
        if (newRoot == nullptr) {
            newRoot = currNode;
        }

        // update position relative to node before
        if (currNode->prev != nullptr) {
            currNode->prev->next = currNode;
        }
    }

    // connect to old nodes
    currNode->next = oldNext;

    if (oldPrev != nullptr) {
        oldPrev->next = newRoot;
    }

    node = newRoot;
}


void tokenizeNode(TokenizerNode *node, TOKEN_TYPE tokenType) {
    tokenizeContentNode(node, tokenType);

    std::cout << tokenizerNodeToString(node) << std::endl << std::endl << std::endl;
}


int tokenize(std::string *srcContents) {
    // init root content node
    TokenizerNode *root = new TokenizerNode;
    root->tokenType = TOKEN_TYPE::CONTENT;
    root->src = srcContents;
    root->start = 0;
    root->end = srcContents->length();
    root->prev = nullptr;
    root->next = nullptr;

    // tokenize for each lexeme
    tokenizeNode(root, TOKEN_TYPE::STRING);

    return 0;
}

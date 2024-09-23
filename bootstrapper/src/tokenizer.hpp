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
    {TOKEN_TYPE::STRING, "\"[a-zA-Z0-9\\s\\}]*\""},
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

struct TokenNode {
    TOKEN_TYPE tokenType;
    std::string *src;
    int start;
    int end;
    TokenNode *next;
    TokenNode *prev;
};

std::string tokenNodeToString(TokenNode *node, bool srcDisplay) {
    std::stringstream ss;

    while(node != nullptr) {
        // this node
        ss << "<" << tokenNames.at(node->tokenType) << " ";
        ss << node->start << " ";
        ss << node->end << ">";
        ss << " -> ";

        // next node
        if (node->prev == nullptr) {
            ss << "<nullptr> ";
        } else {
            ss << "<" << tokenNames.at(node->prev->tokenType) << " ";
            ss << node->prev->start << " ";
            ss << node->prev->end << "> ";
        }
        
        // next node
        if (node->next == nullptr) {
            ss << "<nullptr>";
        } else {
            ss << "<" << tokenNames.at(node->next->tokenType) << " ";
            ss << node->next->start << " ";
            ss << node->next->end << ">";
        }

        ss << "\n";

        if (srcDisplay) {
            std::string src = *node->src;
            std::string srcInd = src.substr(node->start, node->end - node->start);

            ss << srcInd << std::endl;
        }

        node = node->next;
    }

    std::string output = ss.str();

    return output;
}

void tokenizeContentNode(TokenNode *node, TOKEN_TYPE tokenType) {
    std::string lexeme = tokenExpressions.at(tokenType);

    // old state
    TokenNode *oldPrev = node->prev;
    TokenNode *oldNext = node->next;
    TokenNode *newRoot = nullptr;
    TokenNode *currNode = nullptr;

    int oldEnd = node->end;
    std::string *src = node->src;           // will be copied to all nodes

    // src file content for within content node scope
    std::string input = *src;
    input = input.substr(node->start, node->end - node->start);

    // match lexeme
    std::regex regex(lexeme);
    std::smatch match;
    auto begin = std::sregex_iterator(input.begin(), input.end(), regex);
    auto end = std::sregex_iterator();

    // current index of search
    int offset = node->start;
    int currIndex = 0;
    int matchStart = 0;
    int matchEnd = 0;
    bool matchFound = false;

    for (auto it = begin; it != end; ++it) {
        // match info
        matchFound = true;
        match = *it;
        std::string matchContents = match.str(0);
        matchStart = match.position(0);
        matchEnd = matchStart + match.length(0);

        if (currIndex != matchStart) {
            TokenNode *newNode = new TokenNode;
            newNode->tokenType = TOKEN_TYPE::CONTENT;
            newNode->src = src;
            newNode->start = currIndex + offset;
            newNode->end = matchStart + offset;

            // update index
            currIndex = matchStart;

            // update positions
            if (newRoot == nullptr) {
                newRoot = newNode;
                newRoot->prev = oldPrev;
            } else {
                newNode->prev = currNode;
                currNode->next = newNode;
            }

            currNode = newNode;
        }

        // lexical node
        TokenNode *newNode = new TokenNode;
        newNode->tokenType = tokenType;
        newNode->src = src;
        newNode->start = matchStart + offset;
        newNode->end = matchEnd + offset;

        // update index
        currIndex = matchEnd;

        // update position
        if (newRoot == nullptr) {
            newRoot = newNode;
            newRoot->prev = oldPrev;
        } else {
            newNode->prev = currNode;
            currNode->next = newNode;
        }

        currNode = newNode;
    }

    if (currIndex + offset != oldEnd && matchFound) {
        // create and insert post content node
        TokenNode *newNode = new TokenNode;
        newNode->tokenType = TOKEN_TYPE::CONTENT;
        newNode->src = src;
        newNode->start = currIndex + offset;
        newNode->end = oldEnd;

        // update position
        if (newRoot == nullptr) {
            newRoot = newNode;
            newRoot->prev = oldPrev;
        } else {
            currNode->next = newNode;
            newNode->prev = currNode;
        }

        currNode = newNode;
    }


    if (oldPrev != nullptr) {
        oldPrev->next = newRoot;
    } else {
        *node = *newRoot;
    }

    if (oldNext != nullptr) {
        oldNext->prev = currNode;
        currNode->next = oldNext;
    }
}


void tokenizeNode(TokenNode *node, TOKEN_TYPE tokenType) {
    /*
    oldPrev null
    <CONTENT 0 98> -> <nullptr>, <STRING 98 119> ->
        // this is some comment that should be parsed OUT
        import add, sub from math

        main() {
        let a =
    <STRING 98 119> -> <CONTENT 0 98> -> , <CONTENT 119 144> ->
        "what is this string"
    <CONTENT 119 144> -> <STRING 98 119> -> , <STRING 144 168> ->
        }

        main2() {
        let b =
    <STRING 144 168> -> <CONTENT 119 144> -> , <CONTENT 168 170> ->
        "this is another string"
    <CONTENT 168 170> -> <STRING 144 168> -> , <nullptr>

        }
    */
    TokenNode *currNode = node;

    while (currNode != nullptr) {
        TokenNode *nextNode = currNode->next;

        if (currNode->tokenType == TOKEN_TYPE::CONTENT) {
            //tokenizeContentNode(currNode, tokenType);
        }

        currNode = nextNode;
    }
}


int tokenize(std::string *srcContents) {
    // init root content node
    TokenNode *root = new TokenNode;
    root->tokenType = TOKEN_TYPE::CONTENT;
    root->src = srcContents;
    root->start = 0;
    root->end = srcContents->length();
    root->next = nullptr;

    // tokenize for each lexeme
    tokenizeContentNode(root, TOKEN_TYPE::STRING);
    std::cout << tokenNodeToString(root, false) << std::endl << std::endl << std::endl;
    tokenizeContentNode(root->next->next, TOKEN_TYPE::R_CURLY_DELIMETER);
    std::cout << tokenNodeToString(root, false) << std::endl << std::endl << std::endl;
    tokenizeContentNode(root->next->next->next->next->next->next, TOKEN_TYPE::R_CURLY_DELIMETER);
    std::cout << tokenNodeToString(root, false) << std::endl << std::endl << std::endl;
    //tokenizeNode(root, TOKEN_TYPE::R_CURLY_DELIMETER);
    //tokenizeNode(root, TOKEN_TYPE::R_CURLY_DELIMETER);
    //tokenizeNode(root->next->next, TOKEN_TYPE::R_CURLY_DELIMETER);
    //tokenizeNode(root->next->next->next->next->next->next, TOKEN_TYPE::R_CURLY_DELIMETER);
    //tokenizeNode(root, TOKEN_TYPE::L_CURLY_DELIMETER);
    //tokenizeNode(root->next->next, TOKEN_TYPE::R_CURLY_DELIMETER);

    //std::cout << TokenNodeToString(root) << std::endl << std::endl << std::endl;

    return 0;
}

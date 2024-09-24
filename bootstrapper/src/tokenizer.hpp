// Copyright 2024 Michael Vaden
#pragma once

#include <map>
#include <regex>
#include <sstream>
#include <string>
#include "linked_list.hpp"

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
    {TOKEN_TYPE::OP_DOT, "\\."},
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

struct TokenData {
    TOKEN_TYPE tokenType;
    std::string *src;
    int start;
    int end;
};

std::string tokenNodeToString(LLNode<TokenData> *node, bool displayContent) {
    std::stringstream ss;

    if (node != nullptr) {
        ss << "<" << tokenNames.at(node->data->tokenType);
        ss << " " << node->data->start << " ";
        ss << node->data->end;
        ss << ">";

        ss << " - ";

        if (node->prev != nullptr) {
            ss << "<" << tokenNames.at(node->prev->data->tokenType);
            ss << " " << node->prev->data->start << ", ";
            ss << " " << node->prev->data->end << ">, ";
        } else {
            ss << "<null>, ";
        }

        if (node->next != nullptr) {
            ss << "<" << tokenNames.at(node->next->data->tokenType);
            ss << " " << node->next->data->start << ", ";
            ss << " " << node->next->data->end << ">\n";
        } else {
            ss << "<null>\n";
        }

        if (displayContent) {
            std::string display = *node->data->src;
            int i = node->data->start;
            int j = node->data->end - node->data->start;
            ss << display.substr(i, j) << "\n";
        }
    } else {
        ss << "null" << std::endl;
    }

    return ss.str();
}

std::string tokenToString(LLNode<TokenData> *node, bool displayContent) {
    std::stringstream ss;

    LLNode<TokenData> *curr = node;

    while (curr != nullptr) {
        ss << tokenNodeToString(curr, displayContent);
        curr = curr->next;
    }

    return ss.str();
}

void tokenizeContentNode(LLNode<TokenData> *node, TOKEN_TYPE tokenType) {
    // regex mapped to token type
    std::string expression = tokenExpressions.at(tokenType);

    int oldEnd = node->data->end;               // length of src
    std::string *src = node->data->src;         // should be copied to all nodes

    // src file content for within content node scope
    std::string input = *src;
    int i = node->data->start;
    int j = node->data->end - node->data->start;
    input = input.substr(i, j);

    // match expression
    std::regex regex(expression);
    std::smatch match;
    auto begin = std::sregex_iterator(input.begin(), input.end(), regex);
    auto end = std::sregex_iterator();

    // current index of search
    int offset = node->data->start;
    int currIndex = 0;
    int matchStart = 0;
    int matchEnd = 0;
    bool matchFound = false;

    LLNode<TokenData> *rootNode = nullptr;
    LLNode<TokenData> *currNode = nullptr;

    for (auto it = begin; it != end; ++it) {
        // match information
        matchFound = true;
        match = *it;
        std::string matchContents = match.str(0);
        matchStart = match.position(0);
        matchEnd = matchStart + match.length(0);

        // prefix content node
        if (currIndex != matchStart) {
            LLNode<TokenData> *node = new LLNode<TokenData>;
            TokenData *tokenData = new TokenData;
            tokenData->tokenType = TOKEN_TYPE::CONTENT;
            tokenData->src = src;
            tokenData->start = currIndex + offset;
            tokenData->end = matchStart + offset;

            node->data = tokenData;
            node->next = nullptr;
            node->prev = nullptr;

            // update index
            currIndex = matchStart;

            // link node
            if (rootNode == nullptr) {      // first node case
                rootNode = node;
                currNode = rootNode;
            } else {                        // not first
                node->prev = currNode;
                node->prev->next = node;

                currNode = node;
            }
        }

        // lexical node
        LLNode<TokenData> *node = new LLNode<TokenData>;
        TokenData *tokenData = new TokenData;
        tokenData->tokenType = tokenType;
        tokenData->src = src;
        tokenData->start = matchStart + offset;
        tokenData->end = matchEnd + offset;

        node->data = tokenData;
        node->next = nullptr;
        node->prev = nullptr;

        // update index
        currIndex = matchEnd;

        // link node
        if (rootNode == nullptr) {      // first node case
            rootNode = node;
            currNode = rootNode;
        } else {                        // not first
            node->prev = currNode;
            node->prev->next = node;

            currNode = node;
        }
    }

    // suffix content node
    if (currIndex + offset != oldEnd && matchFound) {
        LLNode<TokenData> *node = new LLNode<TokenData>;
        TokenData *tokenData = new TokenData;
        tokenData->tokenType = TOKEN_TYPE::CONTENT;
        tokenData->src = src;
        tokenData->start = currIndex + offset;
        tokenData->end = oldEnd;

        node->data = tokenData;
        node->next = nullptr;
        node->prev = nullptr;

        // link node
        if (rootNode == nullptr) {      // first node case
            rootNode = node;
            currNode = rootNode;
        } else {                        // not first
            node->prev = currNode;
            node->prev->next = node;

            currNode = node;
        }
    }

    // insert new node tree in place of old
    if (rootNode != nullptr) {
        replace(node, rootNode);
    }
}

void tokenizeNode(LLNode<TokenData> *node, TOKEN_TYPE tokenType) {
    LLNode<TokenData> *currNode = node;

    while (currNode != nullptr) {
        if (currNode->data->tokenType == TOKEN_TYPE::CONTENT) {
            tokenizeContentNode(currNode, tokenType);
        }

        currNode = currNode->next;
    }
}

LLNode<TokenData> *tokenize(std::string *srcContents) {
    LLNode<TokenData> *root = new LLNode<TokenData>();
    TokenData *tokenData = new TokenData;
    tokenData->tokenType = TOKEN_TYPE::CONTENT;
    tokenData->src = srcContents;
    tokenData->start = 0;
    tokenData->end = srcContents->length();

    root->data = tokenData;
    root->next = nullptr;
    root->prev = nullptr;

    tokenizeNode(root, TOKEN_TYPE::STRING);
    tokenizeNode(root, TOKEN_TYPE::L_DELIMETER);
    tokenizeNode(root, TOKEN_TYPE::R_DELIMETER);
    tokenizeNode(root, TOKEN_TYPE::L_CURLY_DELIMETER);
    tokenizeNode(root, TOKEN_TYPE::R_CURLY_DELIMETER);
    std::cout << tokenToString(root, true) << std::endl;

    return root;
}

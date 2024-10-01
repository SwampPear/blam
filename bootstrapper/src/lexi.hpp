// Copyright 2024 Michael Vaden

#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

namespace Lexi {

std::string readFile(const std::string& filePath) {
    std::ifstream fileStream(filePath);

    if (!fileStream.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;

        return std::string("");
    }

    std::stringstream buffer;

    buffer << fileStream.rdbuf();

    return std::string(buffer.str());
}

struct TokenData {
    int tokenType;
    std::string *src;
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

typedef std::shared_ptr<Lexi::LLNode> TokenNode;

class Tokenizer {
 public:
    Tokenizer() {
        currentRule = 0;

        addRule("CONTENT", "");     // id 0 reserved for content node
    }

    void addRule(std::string name, std::string expr) {
        Lexeme lexeme {name, expr};
        lexemes[currentRule] = lexeme;

        currentRule++;
    }

    std::shared_ptr<LLNode> tokenize(std::string *src) {
        std::shared_ptr<LLNode> root = createNode(
            0, src, 0, src->length());
        root->next = nullptr;
        root->prev = nullptr;

        for (int i = 1; i < currentRule; i++) {
            tokenizeNode(root, i);
        }

        return root;
    }

    std::string tokenToString(std::shared_ptr<LLNode> node, bool displayContent) {
        std::stringstream ss;

        std::shared_ptr<LLNode> curr = node;

        while (curr != nullptr) {
            ss << tokenNodeToString(curr, displayContent);
            curr = curr->next;
        }

        return ss.str();
    }

 private:
    int currentRule;
    std::unordered_map<int, Lexeme> lexemes;

    std::shared_ptr<LLNode> createNode(int tokenType, std::string *src, int start, int end) {
        std::shared_ptr<LLNode> node = std::make_shared<LLNode>();

        std::shared_ptr<TokenData> tokenData = std::make_shared<TokenData>();
        tokenData->tokenType = tokenType;
        tokenData->src = src;
        tokenData->start = start;
        tokenData->end = end;

        node->data = tokenData;

        return node;
    }

    std::string singleNodeToString(std::shared_ptr<LLNode> node) {
        if (node == nullptr) return "<null>";

        std::stringstream ss;

        ss << "<" << lexemes.at(node->data->tokenType).name << " ";
        ss << node->data->start << " " << node->data->end << ">";

        return ss.str();
    }

    std::string tokenNodeToString(std::shared_ptr<LLNode> node, bool displayContent) {
        if (node == nullptr) return "<null>";

        std::stringstream ss;

        ss << singleNodeToString(node) << " - ";
        ss << singleNodeToString(node->prev) << ", ";
        ss << singleNodeToString(node->next);

        if (displayContent) {
            std::string display = *node->data->src;
            int i = node->data->start;
            int j = node->data->end - node->data->start;
            ss << "\n" << display.substr(i, j) << "\n";
        }

        return ss.str();
    }

    void replace(std::shared_ptr<LLNode> replaced, std::shared_ptr<LLNode> replacement) {
        if (replaced == nullptr) {
            throw std::runtime_error("Replaced node cannot be null.");
        }

        if (replacement == nullptr) {
            throw std::runtime_error("Replacement node cannot be null.");
        }

        // record next node before replacement
        std::shared_ptr<LLNode> oldNext = replaced->next;

        // link replacement to replaced previous node 
        replacement->prev = replaced->prev;

        // locate last node of replacement
        std::shared_ptr<LLNode> currNode = replacement;

        while (currNode->next != nullptr) {
            currNode = currNode->next;
        }

        // replace next node of replacement with replaced next
        currNode->next = oldNext;

        if (oldNext != nullptr) {
            oldNext->prev = currNode;
        }

        // finally replace the replacement
        *replaced = *replacement;
    }

    void tokenizeContentNode(std::shared_ptr<LLNode> node, int tokenType) {
        // regex mapped to token type
        std::string expression = lexemes.at(tokenType).expr;

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

        std::shared_ptr<LLNode> rootNode = nullptr;
        std::shared_ptr<LLNode> currNode = nullptr;

        for (auto it = begin; it != end; ++it) {
            // match information
            matchFound = true;
            match = *it;
            std::string matchContents = match.str(0);
            matchStart = match.position(0);
            matchEnd = matchStart + match.length(0);

            // zero length match should be rejected
            if (matchStart == matchEnd) return;

            // prefix content node
            if (currIndex != matchStart) {
                std::shared_ptr<LLNode> node = createNode(
                    0, src, currIndex + offset, matchStart + offset);

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
            std::shared_ptr<LLNode> node = createNode(
                tokenType, src, matchStart + offset, matchEnd + offset);
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
            std::shared_ptr<LLNode> node = createNode(
                0, src, currIndex + offset, oldEnd);

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

    void tokenizeNode(std::shared_ptr<LLNode> node, int tokenType) {
        std::shared_ptr<LLNode> currNode = node;

        while (currNode != nullptr) {
            if (currNode->data->tokenType == 0) {
                tokenizeContentNode(currNode, tokenType);
            }

            currNode = currNode->next;
        }
    }
};

}  // namespace Lexi
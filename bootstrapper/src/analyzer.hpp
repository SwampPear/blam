#include <cstdlib>
#include <stdexcept>
#include <string>
#include <sstream>
#include <regex>
#include <iostream>

namespace BlamAnalyzer {
/**
 * A lexical rule for token matching.
 */
struct Lexeme {
    std::string name;   // name of Lexeme (i.e. "STRING")
    std::string expr;   // expression to match (i.e. "\"[a-zA-Z0-9\\s\\}]*\"")
};

/**
 * A token for simultaneous use in tokenization and abstract syntax tree 
 * parsing. Represented as a dual linked list/tree representation.
 */
struct Token {
    int id;           // enum type of node
    int start;          // start index in source
    int end;            // end index in source

    Token *next;        // next node in linked list
    Token *prev;        // prev node in linked list
    Token *first;        // first child node
    Token *last;        // last child node
};

/**
 * A string name and an expression for each lexeme.
 */
const Lexeme LEXEMES[] = {
    Lexeme{"CONTENT", ""},
    Lexeme{"STRING", "\"[a-zA-Z0-9\\s\\}]*\""},
    Lexeme{"SINGLE_LINE_COMMENT", "\\/\\/[\\sa-zA-Z0-9]*\n"},
    Lexeme{"MULTI_LINE_COMMENT", "\\/\\*[\\sa-zA-Z0-9]*\\*\\/"},
    Lexeme{"L_DELIMETER", "\\("},
    Lexeme{"R_DELIMETER", "\\)"},
    Lexeme{"L_CURLY_DELIMETER", "\\{"},
    Lexeme{"R_CURLY_DELIMETER", "\\}"},
    Lexeme{"L_SQUARE_DELIMETER", "\\["},
    Lexeme{"R_SQUARE_DELIMETER", "\\]"},
    Lexeme{"OP_PLUS_EQUALS", "\\+="},
    Lexeme{"OP_MINUS_EQUALS", "\\-="},
    Lexeme{"OP_MULTIPLY_EQUALS", "\\*="},
    Lexeme{"OP_DIVIDE_EQUALS", "/="},
    Lexeme{"OP_ARROW", "->"},
    Lexeme{"OP_LEQ", "<="},
    Lexeme{"OP_GEQ", ">="},
    Lexeme{"OP_LT", "<"},
    Lexeme{"OP_GT", ">"},
    Lexeme{"OP_NEQ", "!="},
    Lexeme{"OP_EQQ", "=="},
    Lexeme{"OP_EQ", "="},
    Lexeme{"OP_NOT", "!"},
    Lexeme{"OP_MI", "\\-"},
    Lexeme{"OP_DOT", "\\."},
    Lexeme{"OP_PLUS", "\\+"},
    Lexeme{"OP_MINUS", "\\-"},
    Lexeme{"OP_STAR", "\\*"},
    Lexeme{"OP_SLASH", "/"},
    Lexeme{"OP_COMMA", ","},
    Lexeme{"OP_SCOPE", "::"},
    Lexeme{"OP_COLON", ":"},
    Lexeme{"KEYWORD_RETURN", "return"},
    Lexeme{"KEYWORD_IMPORT", "import"},
    Lexeme{"KEYWORD_PUBLIC", "pub"},
    Lexeme{"KEYWORD_STRUCT", "struct"},
    Lexeme{"KEYWORD_FROM", "from"},
    Lexeme{"KEYWORD_CONST", "const"},
    Lexeme{"KEYWORD_CONST", "let"},
    Lexeme{"SPACE", "\\s+"},
    Lexeme{"ALPHANUM", "^[a-zA-Z][a-zA-Z0-9]*"},
    Lexeme{"NUM", "[0-9]+"},
};
/*
syntax rules
!worth noting function names should be cached during this step
- CALL
add(1, 2)
math.add(1, 2)
print("Asdf")
print()

- OPERATION
1 + 2
x + 2
x + x
x / CALL
a == b

- ASSIGNMENT
i8 x = OPERATION

- FUNCTION
main() {
    ASSIGNMENT
    CALL
}

*/

/**
 * Creates a token.
 * @param id - the lexeme id to assign
 * @param start - the start of the token in source
 * @param end - the end of the token in source
 * @return the new token
 */
Token* createToken(int id, int start, int end) {
    Token* token = new Token;
    token->id = id;
    token->start = start;
    token->end = end;

    return token;
}

/**
 * Replaces a range of tokens with a replacement token.
 * @param first - the first token in the range to replace
 * @param last - the last token in the range to replace
 * @param replacement - the token to replace the range with
 */

void replaceRange(Token* first, Token* last, Token* replacement) {
    // old state
    Token* oldPrev = first->prev;
    Token* oldNext = last->next;

    // link next
    Token* curr = replacement;
    while (curr->next != nullptr) {
        curr = curr->next;
    }

    curr->next = oldNext;

    if (oldNext != nullptr) {
        oldNext->prev = curr;
    }

    // link prev
    if (oldPrev != nullptr) {
        oldPrev->next = replacement;
        replacement->prev = oldPrev;
    }

    *first = *replacement;
}

/**
 * Converts a single token to a string representation for debugging.
 * @param token - the token to convert
 * @param src - source code for the token
 * @param display_content - whether or not to display the content pointed to by
 * the token
 * @return the string representation of the token
 */
std::string singleTokenToString(Token* token, std::string* src, bool display_content) {
    std::stringstream ss;

    // this token to string
    //ss << "<" << LEXEMES[token->id].name << ">";
    ss << "<" << LEXEMES[token->id].name << " ";
    ss << token->start << " " << token->end << ">";

    // prev token to string
    if (token->prev == nullptr) {
        ss << " <null>";
    } else {
        ss << " <" << LEXEMES[token->prev->id].name << " ";
        ss << token->prev->start << " " << token->prev->end << ">";
    }

    // next token to string
    if (token->next == nullptr) {
        ss << " <null>\n";
    } else {
        ss << " <" << LEXEMES[token->next->id].name << " ";
        ss << token->next->start << " " << token->next->end << ">\n";
    }

    // content
    if (display_content) {
        ss << "\n" << src->substr(token->start, token->end - token->start) << "\n";
    }

    return ss.str();
}

/**
 * Converts a token list to a string representation for debugging.
 * @param token - the beginning token to convert
 * @param src - source code for the token
 * @param display_content - whether or not to display the content pointed to by
 * the token
 * @return the string representation of the token
 */
std::string tokenToString(Token* token, std::string* src, bool display_content) {
    std::stringstream ss;

    Token *curr = token;
    while(curr != nullptr) {
        ss << singleTokenToString(curr, src, display_content);
        curr = curr->next;
    }

    return ss.str();
}

/**
 * Tokenizes a content node.
 * @param node - the content node token to tokenize
 * @param src - the source
 * @param id - the id to tokenize for
 */
void tokenizeContentNode(Token* node, std::string* src, int id) {
    // regex mapped to token type
    std::string expression = LEXEMES[id].expr;

    int oldEnd = node->end;

    // src file content for within content node scope
    std::string input = *src;
    input = input.substr(node->start, node->end - node->start);

    // match expression
    std::regex regex(expression);
    std::smatch match;
    auto begin = std::sregex_iterator(input.begin(), input.end(), regex);
    auto end = std::sregex_iterator();

    // current index of search
    int offset = node->start;
    int currIndex = 0;
    int matchStart = 0;
    int matchEnd = 0;
    bool matchFound = false;

    Token* rootNode = nullptr;
    Token* currNode = nullptr;

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
            Token* node = createToken(
                0, currIndex + offset, matchStart + offset);

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

        // BlamTokenizercal node
        Token* node = createToken(
            id, matchStart + offset, matchEnd + offset);
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
        Token* node = createToken(
            0, currIndex + offset, oldEnd);

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
        replaceRange(node, node, rootNode);
    }
}

/**
 * Tokenizes a node range.
 * @param node - the node to begin with
 * @param src - the file source to parse
 * @param id - the id to tokenize for
 */
void tokenizeNode(Token* node, std::string* src, int id) {
    Token* currNode = node;

    while (currNode != nullptr) {
        if (currNode->id == 0) {
            tokenizeContentNode(currNode, src, id);
        }

        currNode = currNode->next;
    }
}

/**
 * Tokenizes over all lexemes.
 * @param src - the source to parse
 * @return the new root token
 */
Token* tokenize(std::string *src) {
    Token* root = createToken(
        0, 0, src->length());
    root->next = nullptr;
    root->prev = nullptr;

    int len = sizeof(LEXEMES) / sizeof(LEXEMES[0]);

    for (int i = 1; i < len; i++) {
        tokenizeNode(root, src, i);
    }

    return root;
}

}  // namespace BlamTokenizer
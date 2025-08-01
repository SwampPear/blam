#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include "tokenizer/tokenizer.hpp"

using namespace Blam;

// Utility to walk a linked list and collect (type, pos, len)
std::vector<std::tuple<Type, uint16_t, uint16_t>> collectTokens(std::shared_ptr<LListNode<Token>> node) {
    std::vector<std::tuple<Type, uint16_t, uint16_t>> result;
    while (node) {
        result.emplace_back(node->data->type, node->data->pos, node->data->len);
        node = node->next;
    }
    return result;
}

TEST_CASE("Single identifier is tokenized", "[tokenizer]") {
    std::string input = "hello";
    auto tokens = tokenize(input);

    auto result = collectTokens(tokens->head);
    REQUIRE(result.size() == 1);
    REQUIRE(std::get<0>(result[0]) == Type::IDENT);
    REQUIRE(std::get<1>(result[0]) == 0);
    REQUIRE(std::get<2>(result[0]) == 5);
}

TEST_CASE("Raw tokens preserved around symbols", "[tokenizer]") {
    std::string input = "x*y";
    auto tokens = tokenize(input);
    auto result = collectTokens(tokens->head);

    REQUIRE(result.size() == 3);
    REQUIRE(std::get<0>(result[0]) == Type::IDENT);
    REQUIRE(std::get<0>(result[1]) == Type::MULT);
    REQUIRE(std::get<0>(result[2]) == Type::IDENT);

    REQUIRE(input.substr(std::get<1>(result[1]), std::get<2>(result[1])) == "*");
}

TEST_CASE("Keywords are recognized", "[tokenizer]") {
    std::string input = "def if el elif while for in ret break continue";
    auto tokens = tokenize(input);
    auto result = collectTokens(tokens->head);

    std::vector<Type> expected = {
        Type::DEF, Type::IF, Type::EL, Type::ELIF,
        Type::WHILE, Type::FOR, Type::IN,
        Type::RET, Type::BREAK, Type::CONTINUE
    };

    for (Type t : expected) {
        bool found = false;
        for (const auto& tok : result) {
            if (std::get<0>(tok) == t) {
                found = true;
                break;
            }
        }
        REQUIRE(found);
    }
}


TEST_CASE("Numbers and decimals are tokenized", "[tokenizer]") {
    std::string input = "42 3.14";
    auto tokens = tokenize(input);
    auto result = collectTokens(tokens->head);

    REQUIRE(result.size() == 3);
    REQUIRE(std::get<0>(result[0]) == Type::NUMBER);
    REQUIRE(std::get<0>(result[2]) == Type::DECIMAL);
}

TEST_CASE("Operators are recognized", "[tokenizer]") {
    std::string input = "+ - * / ** += -= /= *= == != < > <= >=";
    auto tokens = tokenize(input);
    auto result = collectTokens(tokens->head);

    std::vector<Type> expected = {
        Type::PLUS, Type::MIN, Type::MULT, Type::DIV, Type::EXP,
        Type::PLUSEQ, Type::MINEQ, Type::DIVEQ, Type::MULTEQ,
        Type::EQEQ, Type::NEQ, Type::LT, Type::GT, Type::LTE, Type::GTE
    };

    for (Type t : expected) {
        bool found = false;
        for (const auto& tok : result) {
            if (std::get<0>(tok) == t) {
                found = true;
                break;
            }
        }
        REQUIRE(found);
    }
}


TEST_CASE("Delimiters are recognized", "[tokenizer]") {
    std::string input = "()[]{}";
    auto tokens = tokenize(input);
    auto result = collectTokens(tokens->head);

    REQUIRE(std::get<0>(result[0]) == Type::SMBRACKET_L);
    REQUIRE(std::get<0>(result[1]) == Type::SMBRACKET_R);
    REQUIRE(std::get<0>(result[2]) == Type::SQBRACKET_L);
    REQUIRE(std::get<0>(result[3]) == Type::SQBRACKET_R);
    REQUIRE(std::get<0>(result[4]) == Type::CUBRACKET_L);
    REQUIRE(std::get<0>(result[5]) == Type::CUBRACKET_R);
}

TEST_CASE("Strings are recognized", "[tokenizer]") {
    std::string input = R"("hello")";
    auto tokens = tokenize(input);
    auto result = collectTokens(tokens->head);

    REQUIRE(result.size() == 1);
    REQUIRE(std::get<0>(result[0]) == Type::STRING);
}

TEST_CASE("Comments are recognized", "[tokenizer]") {
    std::string input = "# this is a comment\n#* multiline \ncomment *#";
    auto tokens = tokenize(input);
    auto result = collectTokens(tokens->head);

    bool hasSLineComment = false;
    bool hasMLineComment = false;

    for (const auto& tok : result) {
        if (std::get<0>(tok) == Type::SLINE_COMMENT) hasSLineComment = true;
        if (std::get<0>(tok) == Type::MLINE_COMMENT) hasMLineComment = true;
    }

    REQUIRE(hasSLineComment);
    REQUIRE(hasMLineComment);
}


TEST_CASE("Whitespace and newlines", "[tokenizer]") {
    std::string input = "let x = 5\n";
    auto tokens = tokenize(input);
    auto result = collectTokens(tokens->head);

    bool hasLet = false;
    bool hasNewline = false;

    for (const auto& tok : result) {
        if (std::get<0>(tok) == Type::LET) hasLet = true;
        if (std::get<0>(tok) == Type::NLINE) hasNewline = true;
    }

    REQUIRE(hasLet);
    REQUIRE(hasNewline);
}

void expectSequence(const std::shared_ptr<LList<Token>>& list, const std::vector<Type>& expected) {
    auto node = list->head;
    for (Type t : expected) {
        while (node && (node->data->type == Type::WHITESPACE || node->data->type == Type::RAW))
            node = node->next;
        REQUIRE(node != nullptr);
        REQUIRE(node->data->type == t);
        node = node->next;
    }
}

TEST_CASE("Tokenizer handles exponentiation and avoids double MULT", "[tokenizer]") {
    std::string input = "2 ** 3";
    auto tokens = tokenize(input);
    expectSequence(tokens, {Type::NUMBER, Type::EXP, Type::NUMBER});
}

TEST_CASE("Tokenizer handles ambiguous operators", "[tokenizer]") {
    std::string input = "a * * b + c";
    auto tokens = tokenize(input);
    //expectSequence(tokens, {Type::IDENT, Type::MULT, Type::MULT, Type::IDENT, Type::PLUS, Type::IDENT});

    
    std::vector<Type> expected = {
        Type::IDENT, Type::WHITESPACE, Type::MULT, Type::WHITESPACE, Type::MULT, Type::IDENT, Type::PLUS, Type::IDENT
    };

    auto node = tokens->head;
    for (Type t : expected) {
        std::cout << typeToString(node->data->type) << std::endl; 
        // Skip whitespace/raw
        //while (node && (node->data->type == Type::WHITESPACE || node->data->type == Type::RAW))
        //    node = node->next;

        REQUIRE(node != nullptr);

        auto actual = node->data->type;
        auto text = input.substr(node->data->pos, node->data->len);

        INFO("Expected: " << typeToString(t) 
             << " | Got: " << typeToString(actual) 
             << " | Text: '" << text << "'"
             << " | Pos: " << node->data->pos 
             << ", Len: " << node->data->len);

        REQUIRE(actual == t);

        node = node->next;
    }
}

TEST_CASE("Tokenizer handles adjacent identifiers and numbers", "[tokenizer]") {
    std::string input = "x123 456y";
    auto tokens = tokenize(input);
    // This assumes valid IDENTs like "x123" and "456y" will tokenize as RAW if invalid
    REQUIRE(tokens != nullptr);
}

TEST_CASE("Tokenizer handles complex mix", "[tokenizer]") {
    std::string input = "let x = 2**a + b/3.14";
    auto tokens = tokenize(input);
    expectSequence(tokens, {
        Type::LET, Type::IDENT, Type::EQ, Type::NUMBER,
        Type::EXP, Type::IDENT, Type::PLUS,
        Type::IDENT, Type::DIV, Type::DECIMAL
    });
}

TEST_CASE("Tokenizer handles nested spacing and tricky patterns", "[tokenizer]") {
    std::string input = "def   f(x, y)->x**y";
    auto tokens = tokenize(input);
    expectSequence(tokens, {
        Type::DEF, Type::IDENT, Type::SMBRACKET_L, Type::IDENT,
        Type::COMMA, Type::IDENT, Type::SMBRACKET_R, Type::ARROW,
        Type::IDENT, Type::EXP, Type::IDENT
    });
}
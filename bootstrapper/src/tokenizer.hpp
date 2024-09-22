#include <sstream>

// lexemes
enum TOKEN_TYPE {
    STRING,
    CONTENT,
    SINGLE_LINE_COMMENT,
    MULTI_LINE_COMMENT,
    BEGIN_DELIMETER,
    END_DELIMETER,
    BEGIN_CURLY_DELIMETER,
    END_CURLY_DELIMETER,
    BEGIN_SQUARE_DELIMETER,
    END_SQUARE_DELIMETER,
    OP_DOT,
    OP_PLUS,
    OP_MINUS,
    OP_EQUALS,
    OP_SLASH,
    SPACE,
};

const std::string RE_STRING = "\"[a-zA-Z0-9\/\\\s]*\"";
const std::string RE_SINGLE_LINE_COMMENT = "\/\/[\sa-zA-Z0-9]*\n*";
const std::string RE_MULTI_LINE_COMMENT = "\/\*[\sa-zA-Z0-9]*\*\/";
const std::string RE_BEGIN_DELIMETER = "\(";
const std::string RE_END_DELIMETER = "\)";
const std::string RE_BEGIN_CURLY_DELIMETER = "{";
const std::string RE_END_CURLY_DELIMETER = "}";
const std::string RE_BEGIN_SQUARE_DELIMETER = "\[";
const std::string RE_END_SQUARE_DELIMETER = "\]";
const std::string RE_OP_DOT = ".";
const std::string RE_OP_PLUS = "\+";
const std::string RE_OP_MINUS = "-";
const std::string RE_OP_EQUALS = "=";
const std::string RE_OP_SLASH = "/";
const std::string RE_SPACE = "\s*";

const std::string& tokenRe(TOKEN_TYPE tokenType) {
    switch (tokenType) {
    case STRING:
        return RE_STRING;
    case SINGLE_LINE_COMMENT:
        return RE_SINGLE_LINE_COMMENT;

    case MULTI_LINE_COMMENT:
        return RE_MULTI_LINE_COMMENT;

    case BEGIN_DELIMETER:
        return RE_BEGIN_DELIMETER;

    case END_DELIMETER:
        return RE_END_DELIMETER;

    case BEGIN_CURLY_DELIMETER:
        return RE_BEGIN_CURLY_DELIMETER;

    case END_CURLY_DELIMETER:
        return RE_END_CURLY_DELIMETER;

    case BEGIN_SQUARE_DELIMETER:
        return RE_BEGIN_SQUARE_DELIMETER;

    case END_SQUARE_DELIMETER:
        return RE_END_SQUARE_DELIMETER;

    case OP_DOT:
        return RE_OP_DOT;

    case OP_PLUS:
        return RE_OP_PLUS;

    case OP_MINUS:
        return RE_OP_EQUALS;

    case OP_SLASH:
        return RE_OP_SLASH;

    case SPACE:
        return RE_SPACE;

    default:
        return "";
    }
}

struct TokenizerNode {
    TOKEN_TYPE tokenType;
    const std::string& src;
    int start;
    int end;
};

std::string &tokenizerNodeToString(TokenizerNode &tokenNode) {
    std::stringstream ss;

    // Writing to the stringstream
    ss << "<" << tokenNode.tokenType << " ";
    ss << tokenNode.start << " ";
    ss << tokenNode.end << ">\n";

    std::string output = ss.str();

    return output;
}

int tokenize(const std::string& src) {
    return 0;
}

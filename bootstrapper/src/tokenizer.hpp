// lexemes
enum TOKEN_TYPE {
    CONTENT,                // ignore
    SINGLE_LINE_COMMENT,    // ignore
    MULTI_LINE_COMMENT,     // ignore
    BEGIN_DELIMETER,
    END_DELIMETER,
    BEGIN_CURLY_DELIMETER,
    END_CURLY_DELIMETER,
    BEGIN_SQUARE_DELIMETER,
    END_SQUARE_DELIMETER
};

const std::string RE_SINGLE_LINE_COMMENT = "\/\/[\sa-zA-Z0-9]*\n*";
const std::string RE_MULTI_LINE_COMMENT = "\/\*[\sa-zA-Z0-9]*\*\/";
const std::string RE_BEGIN_DELIMETER = "\(";
const std::string RE_END_DELIMETER = "\)";
const std::string RE_BEGIN_CURLY_DELIMETER = "{";
const std::string RE_END_CURLY_DELIMETER = "}";
const std::string RE_BEGIN_SQUARE_DELIMETER = "\[";
const std::string RE_END_SQUARE_DELIMETER = "\]";

const std::string& tokenRe(TOKEN_TYPE tokenType) {
    switch (tokenType) {
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

    default:
        return "";
    }
}

struct TokenizerNode {
    const std::string& src;
    int start;
    int end;
};

int tokenize(const std::string& src) {
    return 0;
}

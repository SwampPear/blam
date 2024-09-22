// lexemes
enum TOKEN {
    CONTENT,
    SINGLE_LINE_COMMENT,
    MULTI_LINE_COMMENT
};

struct TokenizerNode {
    const std::string& src;
    int start;
    int end;
};

const std::string& tokenExpression(TOKEN token) {
    switch (token) {
    case SINGLE_LINE_COMMENT:
        return RE_SINGLE_LINE_COMMENT;

    case MULTI_LINE_COMMENT:
        return RE_MULTI_LINE_COMMENT;

    default:
        return "";
    }
}

const std::string RE_SINGLE_LINE_COMMENT = "\/\/[\sa-zA-Z0-9]*\n*";
const std::string RE_MULTI_LINE_COMMENT = "\/\*[\sa-zA-Z0-9]*\*\/";

int tokenize(const std::string& src) {
    return 0;
}

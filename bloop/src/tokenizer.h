#ifndef TOKENIZER_H
#define TOKENIZER_H

/**
 * Lexemes.
 */
enum Lexeme {
    NONE,
    STRING,
    SINGLE_LINE_COMMENT,
    MULTI_LINE_COMMENT,
    L_DELIMETER,
    R_DELIMETER,
    L_CURLY_DELIMETER,
    R_CURLY_DELIMETER,
    L_SQUARE_DELIMETER,
    R_SQUARE_DELIMETER,
    OP_PLUS_EQUALS,
    OP_MINUS_EQUALS,
    OP_MULTIPLY_EQUALS,
    OP_DIVIDE_EQUALS,
    OP_ARROW,
    OP_LEQ,
    OP_GEQ,
    OP_LT,
    OP_GT,
    OP_NEQ,
    OP_EQQ,
    OP_EQ,
    OP_NOT,
    OP_DOT,
    OP_PLUS,
    OP_MINUS,
    OP_STAR,
    OP_SLASH,
    OP_COMMA,
    OP_SCOPE,
    OP_COLON,
    KEYWORD_RETURN,
    KEYWORD_IMPORT,
    KEYWORD_PUBLIC,
    KEYWORD_STRUCT,
    KEYWORD_FROM,
    KEYWORD_CONST,
    KEYWORD_LET,
    SPACE,
    ALPHANUM,
    NUM
};

/**
 * Lexical rule for tokenization.
 */
typedef struct lexeme_rule {
    int lexeme;
    const char* expr;
} lexeme_rule_t;

/**
 * Token for tokenization and AST parsing.
 */
typedef struct token {
    int lexeme;
    int start;
    int end;

    struct token* next;
    struct token* prev;
    struct token* first;
    struct token* last;
} token_t;

/**
 * Creates a token.
 * @param lexeme - lexeme id
 * @param start - starting index
 * @param end - ending index
 * @return new token
 */
token_t* create_token(int lexeme, int start, int end);

/**
 * Replaces a range of tokens with a replacement range
 * @param first - first token in range
 * @param last - second token in range
 * @param replacement - first token in replacement range
 * @return first token in range
 */
token_t* replace_range(token_t* first, token_t* last, token_t* replacement);

/**
 * String representation of a token.
 * @param token - token
 * @return string representation
 */
char* token_to_string(token_t* token);

#endif  // TOKENIZER_H
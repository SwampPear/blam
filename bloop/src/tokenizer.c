#include <stdlib.h>
#include <stdio.h>
#include "tokenizer.h"

static lexeme_rule_t lexeme_rules[] = {
    {NONE, ""},
    {STRING, "\"[!-~\\s]*\""},
    {SINGLE_LINE_COMMENT, "\\/\\/[!-~\\s]*\n"},
    {MULTI_LINE_COMMENT, "\\/\\*[!-~\\s]*\\*\\/"},
    {L_DELIMETER, "\\("},
    {R_DELIMETER, "\\)"},
    {L_CURLY_DELIMETER, "\\{"},
    {R_CURLY_DELIMETER, "\\}"},
    {L_SQUARE_DELIMETER, "\\["},
    {R_SQUARE_DELIMETER, "\\]"},
    {OP_PLUS_EQUALS, "\\+="},
    {OP_MINUS_EQUALS, "\\-="},
    {OP_MULTIPLY_EQUALS, "\\*="},
    {OP_DIVIDE_EQUALS, "/="},
    {OP_ARROW, "->"},
    {OP_LEQ, "<="},
    {OP_GEQ, ">="},
    {OP_LT, "<"},
    {OP_GT, ">"},
    {OP_NEQ, "!="},
    {OP_EQQ, "=="},
    {OP_EQ, "="},
    {OP_NOT, "!"},
    {OP_DOT, "\\."},
    {OP_PLUS, "\\+"},
    {OP_MINUS, "\\-"},
    {OP_STAR, "\\*"},
    {OP_SLASH, "/"},
    {OP_COMMA, ","},
    {OP_SCOPE, "::"},
    {OP_COLON, ":"},
    {KEYWORD_RETURN, "return"},
    {KEYWORD_IMPORT, "import"},
    {KEYWORD_PUBLIC, "pub"},
    {KEYWORD_STRUCT, "struct"},
    {KEYWORD_FROM, "from"},
    {KEYWORD_CONST, "const"},
    {KEYWORD_CONST, "let"},
    {SPACE, "\\s+"},
    {ALPHANUM, "^[a-zA-Z][a-zA-Z0-9]*"},
    {NUM, "[0-9]+"}
};

token_t* create_token(int lexeme, int start, int end) {
    token_t* token = malloc(sizeof(token_t));
    token->lexeme = lexeme;
    token->start = start;
    token->end = end;

    return token;
}

char* token_to_string(token_t* token) {
    char* str = malloc(sizeof(char) * 100);

    sprintf(str, "Token(lexeme=%d, start=%d, end=%d)", token->lexeme, token->start, token->end);

    return str;
}

token_t* replace_range(token_t* first, token_t* last, token_t* replacement) {
    // old state
    token_t* old_prev = first->prev;
    token_t* old_next = last->next;

    // link next
    token_t* curr = replacement;
    while (curr->next) curr = curr->next;

    curr->next = old_next;
    if (old_next) old_next->prev = curr;
    
    // link prev
    replacement->prev = old_prev;
    if (old_prev) old_prev->next = replacement;

    // unlink old range
    first->prev = NULL;
    last->next = NULL;

    // save old first token
    //token_t* temp = first;

    // set first ptr
    //if (!old_prev) *first = *replacement;

    return first;
}
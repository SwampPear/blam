#include <stdlib.h>
#include <stdio.h>
#include "tokenizer.h"

static lexeme_rule_t lexeme_rules[] = {
    {NONE, "NONE", ""},
    {STRING, "STRING", "\"[!-~\\s]*\""},
    {SINGLE_LINE_COMMENT, "SINGLE_LINE_COMMENT", "\\/\\/[!-~\\s]*\n"},
    {MULTI_LINE_COMMENT, "MULTI_LINE_COMMENT", "\\/\\*[!-~\\s]*\\*\\/"},
    {L_DELIMETER, "L_DELIMETER", "\\("},
    {R_DELIMETER, "R_DELIMETER", "\\)"},
    {L_CURLY_DELIMETER, "L_CURLY_DELIMETER", "\\{"},
    {R_CURLY_DELIMETER, "R_CURLY_DELIMETER", "\\}"},
    {L_SQUARE_DELIMETER, "L_SQUARE_DELIMETER", "\\["},
    {R_SQUARE_DELIMETER, "R_SQUARE_DELIMETER", "\\]"},
    {OP_PLUS_EQUALS, "OP_PLUS_EQUALS", "\\+="},
    {OP_MINUS_EQUALS, "OP_MINUS_EQUALS", "\\-="},
    {OP_MULTIPLY_EQUALS, "OP_MULTIPLY_EQUALS", "\\*="},
    {OP_DIVIDE_EQUALS, "OP_DIVIDE_EQUALS", "/="},
    {OP_ARROW, "OP_ARROW", "->"},
    {OP_LEQ, "OP_LEQ", "<="},
    {OP_GEQ, "OP_GEQ", ">="},
    {OP_LT, "OP_LT", "<"},
    {OP_GT, "OP_GT", ">"},
    {OP_NEQ, "OP_NEQ", "!="},
    {OP_EQQ, "OP_EQQ", "=="},
    {OP_EQ, "OP_EQ", "="},
    {OP_NOT, "OP_NOT", "!"},
    {OP_DOT, "OP_DOT", "\\."},
    {OP_PLUS, "OP_PLUS", "\\+"},
    {OP_MINUS, "OP_MINUS", "\\-"},
    {OP_STAR, "OP_STAR", "\\*"},
    {OP_SLASH, "OP_SLASH", "/"},
    {OP_COMMA, "OP_COMMA", ","},
    {OP_SCOPE, "OP_SCOPE", "::"},
    {OP_COLON, "OP_COLON", ":"},
    {KEYWORD_RETURN, "KEYWORD_RETURN", "return"},
    {KEYWORD_IMPORT, "KEYWORD_IMPORT", "import"},
    {KEYWORD_PUBLIC, "KEYWORD_PUBLIC", "pub"},
    {KEYWORD_STRUCT, "KEYWORD_STRUCT", "struct"},
    {KEYWORD_FROM, "KEYWORD_FROM", "from"},
    {KEYWORD_CONST, "KEYWORD_CONST", "const"},
    {KEYWORD_LET, "KEYWORD_LET", "let"},
    {SPACE, "SPACE", "\\s+"},
    {ALPHANUM, "ALPHANUM", "^[a-zA-Z][a-zA-Z0-9]*"},
    {NUM, "NUM", "[0-9]+"}
};

token_t* create_token(int lexeme, int start, int end) {
    token_t* token = malloc(sizeof(token_t));
    token->lexeme = lexeme;
    token->start = start;
    token->end = end;

    return token;
}

char* token_to_string(token_t* token) {
    char* token_str = malloc(sizeof(char) * 300);
    sprintf(token_str, "<%s, %d, %d>", get_lexeme_name(token->lexeme), token->start, token->end);

    if (token->prev) {
        sprintf(token_str, "%s : <%s, %d, %d>", token_str, get_lexeme_name(token->prev->lexeme), token->prev->start, token->prev->end);
    } else {
        sprintf(token_str, "%s : <null>", token_str);
    }

    if (token->next) {
        sprintf(token_str, "%s -> <%s, %d, %d>", token_str, get_lexeme_name(token->next->lexeme), token->next->start, token->next->end);
    } else {
        sprintf(token_str, "%s -> <null>", token_str);
    }

    return token_str;
}

char* get_lexeme_name(int lexeme) {
    return lexeme_rules[lexeme].name;
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

    // return replaced range
    return first;
}
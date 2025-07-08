#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>

typedef enum{
    TOKEN_TEXT,
    TOKEN_VAR_OPEN,
    TOKEN_VAR_CLOSE,
    TOKEN_IDENTIFIER,
    TOKEN_DOT,
    TOKEN_STATEMENT_OPEN,
    TOKEN_STATEMENT_CLOSE,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_ENDIF,
    TOKEN_FOR,
    TOKEN_ENDFOR,
    TOKEN_IN,
    TOKEN_ERROR,
    TOKEN_INCLUDE,
    TOKEN_QUOTE_OPEN,
    TOKEN_QUOTE_CLOSE,
    TOKEN_STRING_LITERAL,
    TOKEN_EOF,
}TokenType;

typedef struct{
    TokenType type;
    char* str;
}Token;


typedef struct{
    const char* input;
    unsigned int pos;
    unsigned int len;
    bool inside_var_tag;
    bool inside_statement_tag;
    bool inside_quote;
}Lexer;



Lexer* CreateLexer(const char* input);
void FreeLexer(Lexer* l);

Token LexerNextToken(Lexer* l);
Token* LexerGetTokens(Lexer *l, int* outputSize);


#endif
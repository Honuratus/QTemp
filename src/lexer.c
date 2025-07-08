#include "lexer.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>


Lexer* CreateLexer(const char* input){
    Lexer* l = (Lexer*)malloc(sizeof(Lexer));
    if(!l) return NULL;

    l->len = strlen(input);
    l->pos = 0;
    l->input = input;
    l->inside_var_tag = false;
    l->inside_statement_tag = false;
    l->inside_quote = false;
    return l;
}

void advance(Lexer* l){
    l->pos++;
}

char peek(Lexer* l){
    if (l->pos >= l->len) return 26;
    return l->input[l->pos];
}

char peeknext(Lexer* l){
    unsigned int pos = l->pos+1;

    if (pos >= l->len) return 26;
    return l->input[pos];
}

Token LexerNextToken(Lexer* l){
    if(l->pos >= l->len){
        Token token;
        token.type = TOKEN_EOF;
        token.str = NULL;
        return token;
    }
    if(!l->inside_var_tag && !l->inside_statement_tag){
        if(peek(l) == '{' && peeknext(l) == '{'){
            advance(l);
            advance(l);
            l->inside_var_tag = true;
            
            char* s = (char*)malloc(sizeof(char) * 3);
            memcpy(s, "{{",2);
            s[2] = '\0';
    
    
            Token token;
            token.type = TOKEN_VAR_OPEN;
            token.str = s;
            return token;
        }
        else if(peek(l) == '{' && peeknext(l) == '%'){
            advance(l);
            advance(l);
            l->inside_statement_tag = true;

            char* s = (char*)malloc(sizeof(char) * 3);
            memcpy(s, "{%",2);
            s[2] = '\0';
            
            Token token;
            token.type = TOKEN_STATEMENT_OPEN;
            token.str = s;
            return token;
        }
        else
        {
            unsigned int start = l->pos;
            while(peek(l) != 26 && (peek(l) != '{' || peeknext(l) != '{') && (peek(l) != '{' || peeknext(l) != '%')){
                advance(l);
            }
            
            unsigned int len = l->pos - start;
            char* text = (char*)malloc(sizeof(char) * (len+1));
            memcpy(text,l->input + start,len);
            text[len] = '\0';
            

            Token token;
            token.type = TOKEN_TEXT;
            token.str = text;
            return token;
        }
    }
    else if (l->inside_var_tag)
    {
        while(peek(l) == ' ') advance(l);
        if(peek(l) == '}' && peeknext(l) == '}'){
            advance(l);
            advance(l);
            l->inside_var_tag = false;
            
            char* s = (char*)malloc(sizeof(char) * 3);
            memcpy(s, "}}",2);
            s[2] = '\0';
            
            Token token;
            token.type = TOKEN_VAR_CLOSE;
            token.str = s;
            return token;
        }
        else if(peek(l) == '.'){;
            advance(l);
            char* s = (char*)malloc(2);
            memcpy(s, ".", 1);
            s[1] = '\0';

            Token token;
            token.type = TOKEN_DOT;
            token.str = s;
            return token;
        }
        else
        {
            unsigned int start = l->pos;
            while(isalnum(peek(l)) || peek(l) == '_')
                advance(l);

            unsigned int len = l->pos - start;
            
            // CHECK LEN IF 0 RETURN ERROR

            char* identifier = (char*)malloc(sizeof(char) * (len + 1));
            memcpy(identifier, l->input + start, len);
            identifier[len] = '\0';

            

            Token token;
            token.type = TOKEN_IDENTIFIER;
            token.str = identifier;
            return token;
        }
    }
    else if (l->inside_statement_tag)
    {
        while(peek(l) == ' ') advance(l);
        if(peek(l) == '%' && peeknext(l) == '}'){
            advance(l);
            advance(l);
            l->inside_statement_tag = false;

            char* s = (char*)malloc(sizeof(char) * 3);
            memcpy(s, "%}",2);
            s[2] = '\0';
            
            Token token;
            token.type = TOKEN_STATEMENT_CLOSE;
            token.str = s;
            return token;
        }
        else if(peek(l) == '"' && !l->inside_quote){
            advance(l);
            l->inside_quote = true;

            Token token;
            token.type = TOKEN_QUOTE_OPEN;
            
            char* s = (char*)malloc(sizeof(char) * 2);
            memcpy(s, "\"",1);
            s[1] = '\0';

            token.str = s;
            return token;
        }
        else if(l->inside_quote){
            if(peek(l) == '"'){
                advance(l);
                l->inside_quote = false;

                Token token;
                token.type = TOKEN_QUOTE_CLOSE;
                
                char* s = (char*)malloc(sizeof(char) * 2);
                memcpy(s, "\"",1);
                s[1] = '\0';

                token.str = s;
                return token;
            }
            unsigned int start = l->pos;

            while(peek(l) != '"' && peek(l) != '\0')
                advance(l);

            if(peek(l) == '\0'){
                Token token;
                token.type =TOKEN_ERROR;
                token.str = NULL;
                return token;
            }
            
            unsigned int len = l->pos - start;

            char* filename = (char*)malloc(sizeof(char) * (len+1));
            memcpy(filename, l->input + start, len);
            filename[len] = '\0';

            Token token;
            token.type = TOKEN_STRING_LITERAL;
            token.str = filename;
            return token;
        }
        else{
            unsigned int start = l->pos;
            while(isalnum(peek(l)) || peek(l) == '_')
                advance(l);
            
            unsigned int len = l->pos - start;


            char* word = (char*)malloc(sizeof(char) * (len + 1));
            memcpy(word, l->input + start, len);
            word[len] = '\0';

            Token token;
            if(strcmp("if", word) == 0){
                token.type = TOKEN_IF;
                token.str = word;
            }
            else if(strcmp("for", word) == 0){
                token.type = TOKEN_FOR;
                token.str = word;
            }
            else if(strcmp("in", word) == 0){
                token.type = TOKEN_IN;
                token.str = word;
            }
            else if(strcmp("endif", word) == 0){
                token.type = TOKEN_ENDIF;
                token.str = word;
            }
            else if(strcmp("endfor", word) == 0){
                token.type = TOKEN_ENDFOR;
                token.str = word;
            }
            else if(strcmp("else", word) == 0){
                token.type = TOKEN_ELSE;
                token.str = word;
            }
            else{
                token.type = TOKEN_IDENTIFIER;
                token.str = word;
            }
            return token;
        }
    }
    Token token;
    token.str = "error";
    token.type = TOKEN_ERROR;
    return token;
}


Token* LexerGetTokens(Lexer* l, int* outCount) {
    int capacity = 16;
    int count = 0;
    Token* tokens = malloc(sizeof(Token) * capacity);
    if (!tokens) return NULL;

    while (true) {
        Token t = LexerNextToken(l);

        if (count >= capacity) {
            capacity *= 2;
            Token* temp = realloc(tokens, sizeof(Token) * capacity);
            if (!temp) {
                for (int i = 0; i < count; i++) {
                    free(tokens[i].str);
                }
                free(tokens);
                return NULL;
            }
            tokens = temp;
        }

        tokens[count++] = t;

        if (t.type == TOKEN_EOF) {
            break;
        }
    }


    if (outCount) *outCount = count;
    return tokens;
}


void FreeLexer(Lexer *l){
    if(!l) return;
    free(l);
}
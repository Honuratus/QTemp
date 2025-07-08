#ifndef ENGINE_H
#define ENGINE_H

#include "lexer.h"
#include "parser.h"
#include "dict.h"

typedef struct{
    char* buffer;
    unsigned int length;
    unsigned int capacity;
}StringBuilder;


char* renderAST(Node* ASTNodes, Dictionary* context);
char* render(const char* input, Dictionary* context);

#endif
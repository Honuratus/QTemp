#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum{
    TEXT_NODE,
    EXPR_NODE,
    IF_NODE,
    ELSE_NODE,
    FOR_NODE,
    INCLUDE_NODE
}NodeType;

typedef struct Node{
    NodeType type;
    char* value; // used for EXPR_NODE, IF_NODE condition, FOR_NODE loop variable
    char* container;
    struct Node* next;
    struct Node* children; // used for FOR_NODE, IF_NODE, ELSE_NODE Block Body
    struct Node* elseBranch;
}Node;

Node* ParseTemplate(Token* tokens, int tokenCount);
Node* CreateTextNode(const char* text);
Node* CreateExprNode(const char* varName);
Node* CreateIfNode(const char* condition);
Node* CreateElseNode();
Node* CreateForNode(const char* loopVar, const char* condition);
Node* CreateIncludeNode(const char* literal);

void FreeNodes(Node* head);

#endif
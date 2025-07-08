#include "parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern char* strdup(const char* s);

bool isTag(Token* token, int index, const char* tag);

Node* CreateTextNode(const char* text){
    Node* textNode = (Node*)malloc(sizeof(Node));
    if(!textNode) return NULL;
    
    textNode->type = TEXT_NODE;

    textNode->value = strdup(text);
    if(!textNode->value) return NULL;

    textNode->next = NULL;
    textNode->children = NULL;
    textNode->elseBranch = NULL;
    textNode->container = NULL;

    return textNode;
}

Node* CreateExprNode(const char* varName){
    Node* exprNode = (Node*)malloc(sizeof(Node));
    if(!exprNode) return NULL;

    exprNode->type = EXPR_NODE;
    
    exprNode->value = strdup(varName);
    if(!exprNode->value) return NULL;

    exprNode->next = NULL;
    exprNode->children = NULL;
    exprNode->elseBranch = NULL;
    exprNode->container = NULL;

    return exprNode;
}

Node* CreateIfNode(const char* condition){
    Node* ifNode = (Node*)malloc(sizeof(Node));
    if(!ifNode) return NULL;

    ifNode->type = IF_NODE; 

    ifNode->value = strdup(condition);
    if(!ifNode->value) return NULL;

    ifNode->next = NULL;
    ifNode->children = NULL;
    ifNode->elseBranch = NULL;
    ifNode->container = NULL;

    return ifNode;
}

Node* CreateElseNode(){
    Node* elseNode = (Node*)malloc(sizeof(Node));
    if(!elseNode) return NULL;

    elseNode->type = ELSE_NODE; 
    elseNode->value = NULL;
    elseNode->next = NULL;
    elseNode->children = NULL;
    elseNode->elseBranch = NULL;
    elseNode->container = NULL;

    return elseNode;
}

Node* CreateForNode(const char* loopvar, const char* container){
    Node* forNode = (Node*)malloc(sizeof(Node));
    if(!forNode) return NULL;

    forNode->type = FOR_NODE;

    forNode->container = strdup(container);
    if(!forNode->container) return NULL;

    forNode->value = strdup(loopvar);
    if(!forNode->value) return NULL;

    forNode->next = NULL;
    forNode->children = NULL;
    forNode->elseBranch = NULL;
    return forNode;
}

Node* CreateIncludeNode(const char* literal){
    Node* includeNode = (Node*)malloc(sizeof(Node));
    if(!includeNode) return NULL;

    includeNode->type = INCLUDE_NODE;

    includeNode->value = strdup(literal);
    if(!includeNode->value) return NULL;

    includeNode->next = NULL;
    includeNode->container = NULL;
    includeNode->elseBranch = NULL;
    includeNode->children = NULL;
    return includeNode;
}

Node* ParseBlock(Token* tokens,int* currentIndex,int tokenSize, const char** stopTags, int stopTagSize){
    Node *head, *tail;
    head = tail = NULL;

    while (*currentIndex < tokenSize){
        Token currentToken = tokens[*currentIndex];

        if(currentToken.type == TOKEN_TEXT){
            Node* node = CreateTextNode(currentToken.str);

            if(!head){
                head = tail = node;
            }
            else{
                tail->next = node;
                tail = node;
            }

            (*currentIndex)++;
        }
        
        else if(currentToken.type == TOKEN_VAR_OPEN){
            if (*currentIndex + 2 >= tokenSize){
                fprintf(stderr, "Parse Error: incomplete expression at end\n");
                exit(1);
            }
            if(tokens[*currentIndex + 1].type != TOKEN_IDENTIFIER){
                fprintf(stderr, "Parse Error: expected identifier after '{{'\n");
                exit(1);
            }
            
            char buffer[256] = {0};
            strcat(buffer, tokens[*currentIndex + 1].str);

            int i = (*currentIndex) + 2;
            while(i+1 < tokenSize && tokens[i].type == TOKEN_DOT && tokens[i+1].type == TOKEN_IDENTIFIER){
                strcat(buffer, ".");
                strcat(buffer, tokens[i+1].str);
                i += 2;
            }

            if(tokens[i].type != TOKEN_VAR_CLOSE){
                fprintf(stderr, "Parse Error: expected '}}' after identifier\n");
                exit(1);
            }

            Node* node = CreateExprNode(buffer);

            if(!head) head = tail = node;
            else{
                tail->next = node;
                tail = node;
            }

            *currentIndex = i + 1;
        }
        else if(currentToken.type == TOKEN_STATEMENT_OPEN){
            if (*currentIndex + 2 >= tokenSize) {
                fprintf(stderr, "Parse Error: incomplete tag\n");
                exit(1);
            }

            Token tagNameToken = tokens[*currentIndex + 1];

            for (int i = 0; i < stopTagSize; i++) {
                if (strcmp(tagNameToken.str, stopTags[i]) == 0) {
                    return head;
                }
            }

            if(strcmp(tagNameToken.str, "if") == 0){
                if (*currentIndex + 3 >= tokenSize) {
                    fprintf(stderr, "Parse Error: incomplete 'if' tag\n");
                    exit(1);
                }

                const char* condition = tokens[*currentIndex + 2].str;

                *currentIndex += 4; 

                Node* ifNode = CreateIfNode(condition); 
                const char* stops[] = {"else", "endif"};
                ifNode->children = ParseBlock(tokens, currentIndex, tokenSize, stops, 2);

                if(isTag(tokens, *currentIndex, "else")){
                    *currentIndex += 3;
                    Node* elseNode = CreateElseNode();

                    const char* stops[] =  {"endif"};
                    elseNode->children = ParseBlock(tokens, currentIndex, tokenSize, stops, 1);
                    ifNode->elseBranch = elseNode;
                }
                if (!isTag(tokens, *currentIndex, "endif")) {
                    fprintf(stderr, "Parse Error: expected 'endif'\n");
                    exit(1);
                }
                *currentIndex += 3;
                
                if (!head) head = tail = ifNode;
                else{
                    tail->next = ifNode;
                    tail = tail->next;
                }


            }
            else if(strcmp(tagNameToken.str, "for") == 0){
                if (*currentIndex + 5 >= tokenSize) {
                    fprintf(stderr, "Parse Error: incomplete 'for' tag\n");
                    exit(1);
                }

                const char* loopvar = tokens[*currentIndex + 2].str;
                const char* container = tokens[*currentIndex + 4].str;

                *currentIndex += 6;
                Node* forNode = CreateForNode(loopvar,container);

                const char* stops[] = {"endfor"};
                forNode->children = ParseBlock(tokens, currentIndex, tokenSize, stops, 1);

                if (!isTag(tokens, *currentIndex, "endfor")) {
                    fprintf(stderr, "Parse Error: expected 'endfor'\n");
                    exit(1);
                }

                *currentIndex += 3;

                if (!head) head = tail = forNode;
                else{
                    tail->next = forNode;
                    tail = tail->next;
                }
            }
            
            else if(strcmp(tagNameToken.str, "include") == 0){
                if(*currentIndex + 5 >= tokenSize) {
                    fprintf(stderr, "Parse Error: incomplete 'include' tag\n");
                    exit(1);
                }

                const char* filename = tokens[*currentIndex + 3].str;

                *currentIndex += 6;

                Node* includeNode = CreateIncludeNode(filename);

                if (!head) head = tail = includeNode;
                else{
                    tail->next = includeNode;
                    tail = tail->next;
                }
            }

            else {
                fprintf(stderr, "Parse Error: unknown tag '%s'\n", tagNameToken.str);
                exit(1);
            }
        }
        
        else if(currentToken.type == TOKEN_EOF) return head;
        
        else{
            fprintf(stderr, "Parse Error: unexpected token at index %d: %s\n",*currentIndex, tokens[*currentIndex].str);
            exit(1);
        }
    }
    return head;    
}

Node* ParseTemplate(Token* tokens, int tokenSize){
    int currentIndex = 0;
    const char* stopTags[] = {}; // no stop tags at root
    return ParseBlock(tokens, &currentIndex, tokenSize, stopTags, 0);
}


void FreeNodes(Node* head) {
    while (head) {
        Node* next = head->next;
        if (head->value) {
            free(head->value);
        }
        if (head->children) {
            FreeNodes(head->children);
        }
        if(head->elseBranch){
            FreeNodes(head->elseBranch);
        }
        free(head);
        head = next;
    }
}

bool isTag(Token* tokens, int index, const char* tag) {
    return tokens[index].type == TOKEN_STATEMENT_OPEN &&
           strcmp(tokens[index + 1].str, tag) == 0 &&
           tokens[index + 2].type == TOKEN_STATEMENT_CLOSE;
}
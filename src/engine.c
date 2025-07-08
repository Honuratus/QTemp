#include "engine.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void StringBuilderFree(StringBuilder* sb);

#define TEMPLATE_PATH "/home/onurby/cengine/"

StringBuilder* CreateStringBuilder(){
    StringBuilder* sb = (StringBuilder*)malloc(sizeof(StringBuilder));
    if(!sb) return NULL;

    sb->capacity = 128;
    sb->length = 0;
    sb->buffer = (char*)malloc(sizeof(char) * sb->capacity);
    if(!sb->buffer){
        free(sb);
        return NULL;
    }

    sb->buffer[0] = '\0';

    return sb;
}

// if succses return 1 else 0
int StringBuilderAppend(StringBuilder* sb, const char* str){
    unsigned int len = strlen(str);
    if(sb->length + len +1 >= sb->capacity){
        sb->capacity = (sb->length + len + 1) * 2;
        char* newBuffer = (char*)realloc(sb->buffer, sb->capacity);
        if(!newBuffer){
            StringBuilderFree(sb);
            return 0;
        }
        sb->buffer = newBuffer;
    } 
    memcpy(sb->buffer + sb->length, str, len);
    sb->length += len;
    sb->buffer[sb->length] = '\0';
    return 1;
}

void StringBuilderFree(StringBuilder* sb){
    free(sb->buffer);
    free(sb);
}

KeyValuePair* resolveDottedKey(Dictionary* context, const char* dottedKey) {
    char keyCopy[256];
    strncpy(keyCopy, dottedKey, sizeof(keyCopy));
    keyCopy[sizeof(keyCopy) - 1] = '\0';

    char* token = strtok(keyCopy, ".");
    Dictionary* currentDict = context;
    KeyValuePair* kvp = NULL;

    while (token) {
        kvp = DictionaryGet(currentDict, token);
        if (!kvp) return NULL;

        token = strtok(NULL, ".");

        if (token) {
            if (kvp->value.type != TYPE_DICT) return NULL;
            currentDict = kvp->value.data.dict_val;
        } else {
            return kvp;
        }
    }

    return NULL; 
}

char* renderAST(Node* ASTNodes, Dictionary* context){
    StringBuilder* sb = CreateStringBuilder();
    if(!sb) return NULL;

    for(Node* curr = ASTNodes; curr != NULL; curr = curr->next){
        if(curr->type == TEXT_NODE){
            if(!StringBuilderAppend(sb, curr->value)){
                StringBuilderFree(sb);
                return NULL;
            }
        }
        else if(curr->type == EXPR_NODE){
            KeyValuePair* kvp = resolveDottedKey(context, curr->value); 
            if(!kvp){
                StringBuilderFree(sb);
                return NULL;
            }
            
            const char* val;
            char intStr[20];
            
            switch (kvp->value.type)
            {
                case TYPE_INT:
                    snprintf(intStr, sizeof(intStr), "%d", kvp->value.data.int_val);
                    val = intStr;
                    break;
                case TYPE_STRING:
                    val = kvp->value.data.string_val;
                    break;
                default:
                    StringBuilderFree(sb);
                    printf("tek node\n");
                    return NULL;
            }

            if(!StringBuilderAppend(sb, val)){
                StringBuilderFree(sb);
                return NULL;
            }
        }
        else if(curr->type == IF_NODE){
            KeyValuePair* kvp = DictionaryGet(context, curr->value); // get condition
            if(!kvp || kvp->value.type != TYPE_INT){
                StringBuilderFree(sb);
                return NULL;
            }
            bool isTrue = kvp->value.data.int_val;
            Node* branch = NULL;
            if(isTrue){
                branch = curr->children;
            } else {
                if(curr->elseBranch){
                    if(curr->elseBranch->children) {
                        branch = curr->elseBranch->children;
                    } else {
                        continue;
                    }
                } else {
                    continue;
                }
            }
            
            
            char* inner = renderAST(branch, context);

            if(!inner){
                StringBuilderFree(sb);
                return NULL;
            }

            if(!StringBuilderAppend(sb, inner)){
                StringBuilderFree(sb);
                free(inner);
                return NULL;
            }

            free(inner);

        }
        else if (curr->type == FOR_NODE){
            KeyValuePair* kvp = DictionaryGet(context, curr->container);
            if(!kvp){
                StringBuilderFree(sb);
                return NULL;
            }

            if(kvp->value.type != TYPE_CONTAINER){
                StringBuilderFree(sb);
                return NULL;
            }

            int i;
            for(i = 0; i<kvp->value.data.container_val.count; i++){
                if(!DictionaryAdd(context, curr->value, kvp->value.data.container_val.item[i])){
                    StringBuilderFree(sb);
                    return NULL;
                }

                char* inner = renderAST(curr->children, context);
                if(!inner){
                    DictionaryRemove(context, curr->value);
                    StringBuilderFree(sb);
                    return NULL;
                }

               if(!StringBuilderAppend(sb, inner)){
                    StringBuilderFree(sb);
                    free(inner);
                    return NULL;
                }

                free(inner);

                if(!DictionaryRemove(context, curr->value)){
                    StringBuilderFree(sb);
                    return NULL;
                }
            }
        }
        else if(curr->type == INCLUDE_NODE){
            size_t base_len = strlen(TEMPLATE_PATH);
            size_t val_len = strlen(curr->value);
            size_t full_path_len = base_len + 1 + val_len + 1; // + 2 for zero and slash 
        
            char* full_path = (char*)malloc(full_path_len);
            if(!full_path){
                fprintf(stderr, "Memory allocation failed for full path\n");
                return NULL;
            }
        
            snprintf(full_path, full_path_len, "%s%s", TEMPLATE_PATH, curr->value);

            FILE* f = fopen(full_path, "r");
            free(full_path);
            if(!f){
                fprintf(stderr, "Error: could not open file %s\n", curr->value);
                return NULL;
            }
        
            fseek(f, 0, SEEK_END);
            size_t size = ftell(f);
            rewind(f);
        
            char* content = (char*)malloc(size + 1);
            if (!content) {
                fprintf(stderr, "Memory allocation failed for file content\n");
                fclose(f);
                return NULL;
            }
        
            if (fread(content, 1, size, f) != size) {
                fprintf(stderr, "Error reading file %s\n", curr->value);
                free(content);
                fclose(f);
                return NULL;
            }
            content[size] = '\0';
        
            fclose(f);
        
            char* newval = render(content, context);
            free(content);
            if (!newval) {
                fprintf(stderr, "Error rendering included file %s\n", curr->value);
                return NULL;
            }
        
            if(!StringBuilderAppend(sb, newval)){
                fprintf(stderr,"String Builder Append Error\n");
                StringBuilderFree(sb);
                free(newval);
                return NULL;
            }
        
            free(newval);
        }
    }

    char* output = (char*)malloc(sizeof(char) * (sb->length+1));
    if(!output){
        StringBuilderFree(sb);
        return NULL;
    }


    memcpy(output, sb->buffer, sb->length);
    output[sb->length] = '\0';
    StringBuilderFree(sb);
 
    return output;
}

char* render(const char* input, Dictionary* context){
    Lexer* l = CreateLexer(input);
    int tokenLen = 0;
    Token* t = LexerGetTokens(l, &tokenLen);


    Node* head =  ParseTemplate(t, tokenLen);
    char* output = renderAST(head, context);

    FreeLexer(l);
    FreeNodes(head);
    free(t);
    return output;
}
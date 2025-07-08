#include "dict.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void ValueFree(Value* value);
Value ValueCopy(Value original);



char* strdup(const char* s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char* copy = malloc(len);
    if (copy == NULL) return NULL;
    memcpy(copy, s, len);
    return copy;
}

Dictionary* CreateDictionary(int initial_capacity){
    Dictionary* dict = (Dictionary*)malloc(sizeof(Dictionary));
    if(!dict) return NULL;

    dict->count = 0;
    dict->capacity = initial_capacity;

    dict->pairs = (KeyValuePair*)malloc(sizeof(KeyValuePair) * dict->capacity);
    if(!dict->pairs){
        free(dict);
        return NULL;
    }

    return dict;
}


int resizeDictionary(Dictionary* dict) {
    int new_capacity = dict->capacity * 2;
    KeyValuePair* new_pairs = realloc(dict->pairs, sizeof(KeyValuePair) * new_capacity);
    if (!new_pairs) return 0;
    dict->pairs = new_pairs;
    dict->capacity = new_capacity;
    return 1;
}

Value ValueCopy(Value original){
    Value copy;
    copy.type = original.type;

    switch (original.type)
    {
    case TYPE_INT:
        copy.data.int_val = original.data.int_val;
        break;
    case TYPE_STRING:
        copy.data.string_val = strdup(original.data.string_val);
        break;
    case TYPE_CONTAINER:
        int n = original.data.container_val.count;
        copy.data.container_val.count = n;
        copy.data.container_val.item = malloc(sizeof(Value) * n);
        
        // malloc errors will be handled better
        if(!copy.data.container_val.item){
            copy.data.container_val.count = 0;
            break;
        }
        
        int i;
        for (i=0; i<n; i++) {
            copy.data.container_val.item[i] = ValueCopy(original.data.container_val.item[i]);
        }
        break;
    case TYPE_DICT:
        n = original.data.dict_val->count;
        copy.data.dict_val = CreateDictionary(n);

        for(i = 0; i<n; i++){
            if(!DictionaryAdd(copy.data.dict_val, original.data.dict_val->pairs[i].key, original.data.dict_val->pairs[i].value)){
                FreeDictionary(copy.data.dict_val);
                copy.data.dict_val = NULL;
                break;
            }
        }

        break;
    }

    return copy;
}


// return 1 if succes else 0
int DictionaryAdd(Dictionary* dict, const char* key, Value value){

    // early return for NULL input
    if(key == NULL) return 0; 

    // check if key exists
    int i;
    for(i = 0; i< dict->count; i++){
        if(strcmp(dict->pairs[i].key, key) == 0){
            ValueFree(&dict->pairs[i].value); 
            dict->pairs[i].value = ValueCopy(value);
            return 1;
        }
    }
    
    // resize if dict is full
    // return 0 if resizeDictionary returns error
    if(dict->count == dict->capacity){
        if(!resizeDictionary(dict)){
            return 0;
        }
    }

    // add new keypair
    dict->pairs[dict->count].key = strdup(key);
    dict->pairs[dict->count].value = ValueCopy(value);


    // increment dict count
    dict->count++;

    return 1;
}

int DictionaryRemove(Dictionary* dict, const char* key){
    if (!dict || !key) return 0;

    for(int i = 0; i < dict->count; i++){
        if(strcmp(dict->pairs[i].key, key) == 0){
            // Free the key and value
            free(dict->pairs[i].key);
            ValueFree(&dict->pairs[i].value);

            // Shift remaining pairs left by one
            for(int j = i; j < dict->count - 1; j++){
                dict->pairs[j] = dict->pairs[j + 1];
            }

            dict->count--;
            return 1; // removed successfully
        }
    }

    return 0; // key not found
}

// search for key 
// if not exists return NULL
// else return KVP
KeyValuePair* DictionaryGet(Dictionary* dict, const char* key){
    int i;
    for(i=0; i<dict->count; i++){
        if(strcmp(dict->pairs[i].key, key) == 0){
            return &dict->pairs[i]; // return pointer 
        }
    }
    return NULL; // not found
}


int DictionaryAddInt(Dictionary* dict, const char* key, int val) {
    return DictionaryAdd(dict, key, ValueFromInt(val));
}

int DictionaryAddString(Dictionary* dict, const char* key, const char* val) {
    return DictionaryAdd(dict, key, ValueFromString(val));
}

int DictionaryAddDict(Dictionary* dict, const char* key, Dictionary* val) {
    return DictionaryAdd(dict, key, ValueFromDict(val));
}

int DictionaryAddContainer(Dictionary* dict, const char* key, Container val){
    return DictionaryAdd(dict, key, ValueFromContainer(val));
}



void ValueFree(Value* value) {
    if(value->type == TYPE_STRING){
        free(value->data.string_val);
    }
    else if(value->type == TYPE_CONTAINER){
        int i;
        for(i=0; i<value->data.container_val.count; i++){
            ValueFree(&value->data.container_val.item[i]);
        }

        free(value->data.container_val.item);
    }
}

// free all dynmaically allocated memory
void FreeDictionary(Dictionary* dict){
    if(!dict) return;

    int i;
    for(i=0; i<dict->count; i++){
        free(dict->pairs[i].key);
        ValueFree(&dict->pairs[i].value);
    }
    free(dict->pairs);
    free(dict);
}


Container CreateContainer(int count, ...) {
    Container con;
    con.count = count;
    con.item = malloc(sizeof(Value) * count);
    if (!con.item) {
        con.count = 0;
        return con;
    }

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        Value v = va_arg(args, Value);
        con.item[i] = v;
    }

    va_end(args);
    return con;
}

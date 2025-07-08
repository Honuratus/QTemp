#ifndef DICT_H
#define DICT_H

typedef struct Value Value;
typedef struct Dictionary Dictionary;

typedef enum{
    TYPE_INT,
    TYPE_STRING,
    TYPE_CONTAINER,
    TYPE_DICT,
}ValueType;


typedef struct{
    Value* item;
    int count;
}Container;

struct Value{
    ValueType type;
    union {
        int int_val;
        char* string_val;
        Container container_val;
        Dictionary* dict_val;
    } data;
};

typedef struct{
    char* key;
    Value value;
}KeyValuePair;

struct Dictionary{
    KeyValuePair* pairs;
    int count;
    int capacity;
};


Dictionary* CreateDictionary(int initial_capacity);
int DictionaryAdd(Dictionary* dict, const char* key, Value value);

int DictionaryAddInt(Dictionary* dict, const char* key, int val);
int DictionaryAddString(Dictionary* dict, const char* key, const char* val);
int DictionaryAddDict(Dictionary* dict, const char* key, Dictionary* val);
int DictionaryAddContainer(Dictionary* dict, const char* key, Container con);



Container CreateContainer(int count, ...);

int DictionaryRemove(Dictionary* dict, const char* key);
KeyValuePair* DictionaryGet(Dictionary* dict, const char* key);
void FreeDictionary(Dictionary* dict);
char* strdup(const char* s);

// Create a Value from int
static inline Value ValueFromInt(int x) {
    Value v;
    v.type = TYPE_INT;
    v.data.int_val = x;
    return v;
}

// Create a Value from string (assumes strdup is available)
static inline Value ValueFromString(const char* s) {
    Value v;
    v.type = TYPE_STRING;
    v.data.string_val = strdup(s);  // Make a copy to own
    return v;
}

// Create a Value from a Dictionary pointer
static inline Value ValueFromDict(Dictionary* d) {
    Value v;
    v.type = TYPE_DICT;
    v.data.dict_val = d;
    return v;
}

// Create a Value from Container (you can extend this as needed)
static inline Value ValueFromContainer(Container c) {
    Value v;
    v.type = TYPE_CONTAINER;
    v.data.container_val = c;
    return v;
}


#endif
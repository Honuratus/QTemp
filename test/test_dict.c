#include <stdio.h>
#include <string.h>
#include "dict.h"
#include "minunit.h"

static const char *test_add_and_get() {
    Dictionary *dict = CreateDictionary(2);

    Value v;
    v.type = TYPE_STRING;
    v.data.string_val = strdup("hello");

    DictionaryAdd(dict, "greeting", v);

    KeyValuePair *kv = DictionaryGet(dict, "greeting");
    mu_assert("Key not found", kv != NULL);
    mu_assert("Value wrong", strcmp(kv->value.data.string_val, "hello") == 0);

    FreeDictionary(dict);
    return 0;
}

static const char *test_overwrite_value() {
    Dictionary *dict = CreateDictionary(2);

    Value v1 = {.type = TYPE_INT, .data.int_val = 42};
    DictionaryAdd(dict, "answer", v1);

    Value v2 = {.type = TYPE_INT, .data.int_val = 99};
    DictionaryAdd(dict, "answer", v2); // overwrite

    KeyValuePair *kv = DictionaryGet(dict, "answer");
    mu_assert("Overwrite failed", kv->value.data.int_val == 99);

    FreeDictionary(dict);
    return 0;
}


static const char *test_add_nested_dict() {
    // Create the inner dictionary
    Dictionary *inner = CreateDictionary(2);
    Value inner_val = {.type = TYPE_STRING, .data.string_val = strdup("nested value")};
    DictionaryAdd(inner, "inner_key", inner_val);


    Value v;
    v.type = TYPE_DICT;
    v.data.dict_val = inner;


    Dictionary *outer = CreateDictionary(2);
    DictionaryAdd(outer, "outer_key", v);

    KeyValuePair *outer_kv = DictionaryGet(outer, "outer_key");
    mu_assert("Outer key not found", outer_kv != NULL);
    mu_assert("Outer value not a dict", outer_kv->value.type == TYPE_DICT);

    Dictionary *retrieved_inner = outer_kv->value.data.dict_val;
    mu_assert("Inner dictionary NULL", retrieved_inner != NULL);

    KeyValuePair *inner_kv = DictionaryGet(retrieved_inner, "inner_key");
    mu_assert("Inner key not found", inner_kv != NULL);
    mu_assert("Inner value mismatch", strcmp(inner_kv->value.data.string_val, "nested value") == 0);

    FreeDictionary(outer); 

    return 0;
}


/*
    {% for post in posts %}
        <h1> {{ post.title }} </h1>
    {% endfor %}

*/

const char *test_dict_all() {
    mu_run_test(test_add_and_get);
    mu_run_test(test_overwrite_value);
    mu_run_test(test_add_nested_dict);
    return 0;
}



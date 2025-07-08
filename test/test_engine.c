#include "minunit.h"
#include "engine.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const char* test_render_text_only() {
    // Create AST: TEXT_NODE "Hello", TEXT_NODE " World!"
    Node* n1 = CreateTextNode("Hello");
    Node* n2 = CreateTextNode(" World!");
    n1->next = n2;

    Dictionary* dict = CreateDictionary(4);

    char* result = renderAST(n1, dict);
    mu_assert("Text-only render failed", strcmp(result, "Hello World!") == 0);

    free(result);
    FreeNodes(n1);
    FreeDictionary(dict);
    return NULL;
}

const char* test_render_expr_success() {
    // AST: TEXT_NODE "Hi ", EXPR_NODE "name", TEXT_NODE "!"
    Node* n1 = CreateTextNode("Hi ");
    Node* n2 = CreateExprNode("name");
    Node* n3 = CreateTextNode("!");
    n1->next = n2;
    n2->next = n3;

    Dictionary* dict = CreateDictionary(4);
    Value val;
    val.type = TYPE_STRING;
    val.data.string_val = strdup("ChatGPT");
    DictionaryAdd(dict, "name", val);

    char* result = renderAST(n1, dict);
    mu_assert("Expression render failed", strcmp(result, "Hi ChatGPT!") == 0);

    free(result);
    FreeNodes(n1);
    FreeDictionary(dict);
    return NULL;
}

const char* test_render_missing_key() {
    // AST: EXPR_NODE "missing"
    Node* n1 = CreateExprNode("missing");

    Dictionary* dict = CreateDictionary(4);

    char* result = renderAST(n1, dict);
    mu_assert("Should return NULL for missing key", result == NULL);

    FreeNodes(n1);
    FreeDictionary(dict);
    return NULL;
}

const char* test_render_empty_ast() {
    Node* ast = NULL;

    Dictionary* dict = CreateDictionary(4);

    char* result = renderAST(ast, dict);
    mu_assert("Empty AST should return empty string", result != NULL && strcmp(result, "") == 0);

    free(result);
    FreeDictionary(dict);
    return NULL;
}


const char* test_render_if_true() {
    // AST: IF_NODE "cond"
    // if cond == true, render "Yes"
    Node* ifNode = CreateIfNode("cond");
    ifNode->children = CreateTextNode("Yes");

    Dictionary* dict = CreateDictionary(4);
    Value val;
    val.type = TYPE_INT;
    val.data.int_val = 1;  // true
    DictionaryAdd(dict, "cond", val);

    char* result = renderAST(ifNode, dict);
    mu_assert("IF_NODE true branch failed", strcmp(result, "Yes") == 0);

    free(result);
    FreeNodes(ifNode);
    FreeDictionary(dict);
    return NULL;
}

const char* test_render_if_false_with_else() {
    // AST: IF_NODE "cond" with elseBranch rendering "No"

    Node* ifNode = CreateIfNode("cond");
    ifNode->children = CreateTextNode("Yes");

    Node* elseNode = CreateElseNode(); // else branch (no condition)
    elseNode->children = CreateTextNode("No");
    ifNode->elseBranch = elseNode;



    Dictionary* dict = CreateDictionary(4);
    Value val;
    val.type = TYPE_INT;
    val.data.int_val = 0;  // false
    DictionaryAdd(dict, "cond", val);



    char* result = renderAST(ifNode, dict);
    mu_assert("IF_NODE else branch failed", strcmp(result, "No") == 0);

    free(result);
    FreeNodes(ifNode);  // FreeNodes should handle elseBranch recursively
    FreeDictionary(dict);
    return NULL;
}

const char* test_render_if_false_no_else() {
    // AST: IF_NODE "cond" with no elseBranch

    Node* ifNode = CreateIfNode("cond");
    ifNode->children = CreateTextNode("Yes");

    Dictionary* dict = CreateDictionary(4);
    Value val;
    val.type = TYPE_INT;
    val.data.int_val = 0;  // false
    DictionaryAdd(dict, "cond", val);

    char* result = renderAST(ifNode, dict);
    mu_assert("IF_NODE false without else should render empty string",
              result != NULL && strcmp(result, "") == 0);


    free(result);
    FreeNodes(ifNode);
    FreeDictionary(dict);
    return NULL;
}

const char* test_for() {
    Node* forNode = CreateForNode("post", "posts");
    forNode->children = CreateTextNode("1");

    Dictionary* dict = CreateDictionary(4);

    Value val;
    val.type = TYPE_CONTAINER;
    val.data.container_val.count = 5;
    val.data.container_val.item = (Value*)malloc(sizeof(Value) * 5);

    Value valint;
    valint.type = TYPE_INT;
    valint.data.int_val = 5;

    for (int i = 0; i < 5; i++) {
        val.data.container_val.item[i] = valint;
    }

    DictionaryAdd(dict, "posts", val);

    char* result = renderAST(forNode, dict);
    mu_assert("FOR_NODE invalid string", result != NULL && strcmp(result, "11111") == 0);

    free(result);
    FreeNodes(forNode);
    FreeDictionary(dict);
    return NULL;
}



const char* test_evaluate_dotted_expression() {
    // Setup AST node for expression "post.name"
    Node* exprNode = CreateExprNode("post.name.surname");
    // Setup nested dictionary for context
    Dictionary* innerDict = CreateDictionary(4);

    Value val;
    val.type = TYPE_STRING;
    val.data.string_val = "Yilmaz";
    DictionaryAdd(innerDict, "surname", val);

    
    Dictionary* outerDict = CreateDictionary(4);

    Value val_d;
    val_d.type = TYPE_DICT;
    val_d.data.dict_val = innerDict;

    DictionaryAdd(outerDict, "name", val_d);


    Dictionary* context = CreateDictionary(4);

    Value val_dd;
    val_dd.type = TYPE_DICT;
    val_dd.data.dict_val = outerDict;

    DictionaryAdd(context, "post", val_dd);

    // Evaluate expression node against context
    char* result = renderAST(exprNode, context);



    mu_assert("Evaluation result should not be NULL", result != NULL);
    mu_assert("Evaluation result should match nested value", strcmp(result, "Yilmaz") == 0);

    // Cleanup
    
    FreeNodes(exprNode);
    FreeDictionary(context);
    free(result);

    return NULL;
}


// dict -> container -> dict -> title
//                   -> dict -> title
const char* test_for_loop_with_dict_items() {
    Value title1;
    title1.type = TYPE_STRING;
    title1.data.string_val = "First Post";

    Dictionary* item1 = CreateDictionary(2);
    DictionaryAdd(item1, "title", title1);

    Dictionary* item2 = CreateDictionary(2);
    Value title2;
    title2.type = TYPE_STRING;
    title2.data.string_val = "Second Post";
    DictionaryAdd(item2, "title", title2);

    Value* c = (Value*)malloc(sizeof(Value) * 2);
    c[0].data.dict_val = item1;
    c[0].type = TYPE_DICT;

    c[1].data.dict_val = item2;
    c[1].type = TYPE_DICT;

    Value con;
    con.type = TYPE_CONTAINER;
    con.data.container_val.item = c;
    con.data.container_val.count = 2;



    Dictionary* context = CreateDictionary(4);
    DictionaryAdd(context, "items", con);

    

    Node* forNode = CreateForNode("item", "items");
    forNode->children = CreateTextNode("<h1>");
    forNode->children->next = CreateExprNode("item.title");
    forNode->children->next->next = CreateTextNode("</h1>\n");


    char* result = renderAST(forNode,context);


    const char* expected =
        "<h1>First Post</h1>\n"
        "<h1>Second Post</h1>\n";

    mu_assert("Rendered result should match expected", strcmp(result, expected) == 0);


    FreeNodes(forNode);
    FreeDictionary(context);
    free(result);
    return NULL;
}


const char* test_engine_include() {
    // Create dictionary/context with some variables
    Dictionary* dict = CreateDictionary(2);
    DictionaryAddString(dict, "name", "Onur");

    // Create a temporary included file with some content
    const char* include_filename = "include_test.tpl";
    FILE* f = fopen(include_filename, "w");
    if (!f) return "Failed to create include test file";

    // The included file content uses the variable 'name'
    fprintf(f, "Hello {{ name }} from included file!");
    fclose(f);


    // Template string uses include tag
    const char* template = "Before include. {% include \"include_test.tpl\" %} After include.";


    char* output = render(template, dict);


    // Expected output is the full combined string
    const char* expected = "Before include. Hello Onur from included file! After include.";

    mu_assert("Rendered output mismatch", strcmp(output, expected) == 0);

    // Cleanup
    free(output);
    FreeDictionary(dict);

    // Delete the temporary include file
    remove(include_filename);

    return 0;
}


const char* test_engine_all() {    

    mu_run_test(test_render_text_only);
    mu_run_test(test_render_empty_ast);
    mu_run_test(test_render_expr_success);
    mu_run_test(test_render_missing_key);
    mu_run_test(test_render_if_true);
    mu_run_test(test_render_if_false_with_else);
    mu_run_test(test_render_if_false_no_else);
    mu_run_test(test_for);
    mu_run_test(test_for_loop_with_dict_items);
    mu_run_test(test_evaluate_dotted_expression);
    mu_run_test(test_engine_include);
    return 0;
}


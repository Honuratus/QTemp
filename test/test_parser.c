#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "minunit.h"
#include <stdlib.h>

// Helper to free linked list
void freeNodes(Node* head) {
    while(head) {
        Node* next = head->next;
        free(head->value);
        free(head);
        head = next;
    }
}

// Helper to compare node values with expected string
const char* checkNode(Node* node, NodeType expectedType, const char* expectedValue) {
    mu_assert("Node is NULL", node != NULL);
    mu_assert("Node type mismatch", node->type == expectedType);
    mu_assert("Node value mismatch", strcmp(node->value, expectedValue) == 0);
    return NULL;
}

const char* test_parse_simple_template() {
    // Simulate tokens for "Hello, {{ name }}!"
    Token tokens[] = {
        {TOKEN_TEXT, "Hello, "},
        {TOKEN_VAR_OPEN, "{{"},
        {TOKEN_IDENTIFIER, "name"},
        {TOKEN_VAR_CLOSE, "}}"},
        {TOKEN_TEXT, "!"}
    };
    int tokenCount = sizeof(tokens) / sizeof(tokens[0]);

    Node* ast = ParseTemplate(tokens, tokenCount);

    mu_assert("AST is NULL", ast != NULL);

    // Check first node: TEXT "Hello, "
    const char* msg = checkNode(ast, TEXT_NODE, "Hello, ");
    if(msg) return msg;

    // Second node: EXPR "name"
    mu_assert("Second node NULL", ast->next != NULL);
    msg = checkNode(ast->next, EXPR_NODE, "name");
    if(msg) return msg;

    // Third node: TEXT "!"
    mu_assert("Third node NULL", ast->next->next != NULL);
    msg = checkNode(ast->next->next, TEXT_NODE, "!");
    if(msg) return msg;

    // Should be no more nodes
    mu_assert("More nodes than expected", ast->next->next->next == NULL);

    freeNodes(ast);
    return NULL;
}

const char* test_parse_empty_template() {
    Token tokens[] = {};
    int tokenCount = 0;

    Node* ast = ParseTemplate(tokens, tokenCount);
    mu_assert("AST should be NULL for empty input", ast == NULL);
    return NULL;
}

const char* test_parse_if_else_template() {
    Token tokens[] = {
        {TOKEN_STATEMENT_OPEN, "{%"},
        {TOKEN_IF, "if"},
        {TOKEN_IDENTIFIER, "loggedIn"},
        {TOKEN_STATEMENT_CLOSE, "%}"},
        {TOKEN_TEXT, "Welcome back!"},
        {TOKEN_STATEMENT_OPEN, "{%"},
        {TOKEN_ELSE, "else"},
        {TOKEN_STATEMENT_CLOSE, "%}"},
        {TOKEN_TEXT, "Please log in."},
        {TOKEN_STATEMENT_OPEN, "{%"},
        {TOKEN_ENDIF, "endif"},
        {TOKEN_STATEMENT_CLOSE, "%}"},
        {TOKEN_EOF, NULL}
    };

    int tokenCount = sizeof(tokens) / sizeof(tokens[0]);
    Node* ast = ParseTemplate(tokens, tokenCount);
    mu_assert("AST is NULL", ast != NULL);
    mu_assert("AST should be IF_NODE", ast->type == IF_NODE);
    mu_assert("Condition should be 'loggedIn'", strcmp(ast->value, "loggedIn") == 0);

    mu_assert("IF body should be non-null", ast->children != NULL);
    mu_assert("IF body should be TEXT_NODE", ast->children->type == TEXT_NODE);
    mu_assert("IF body text should be 'Welcome back!'", strcmp(ast->children->value, "Welcome back!") == 0);

    mu_assert("Else branch should exist", ast->elseBranch != NULL);
    mu_assert("Else branch should be TEXT_NODE", ast->elseBranch->children->type == TEXT_NODE);
    mu_assert("Else branch text should be 'Please log in.'", strcmp(ast->elseBranch->children->value, "Please log in.") == 0);

    FreeNodes(ast);
    return NULL;
}

const char* test_parse_dot_notation_expression() {
    Token tokens[] = {
        {TOKEN_VAR_OPEN, "{{"},
        {TOKEN_IDENTIFIER, "post"},
        {TOKEN_DOT, "."},
        {TOKEN_IDENTIFIER, "name"},
        {TOKEN_DOT, "."},
        {TOKEN_IDENTIFIER, "name"},
        {TOKEN_DOT, "."},
        {TOKEN_IDENTIFIER, "name"},
        {TOKEN_DOT, "."},
        {TOKEN_IDENTIFIER, "name"},
        {TOKEN_VAR_CLOSE, "}}"}
    };

    int tokenCount = sizeof(tokens) / sizeof(tokens[0]);
    Node* ast = ParseTemplate(tokens, tokenCount);
    mu_assert("AST is NULL", ast != NULL);
    mu_assert("AST should be EXPR_NODE", ast->type == EXPR_NODE);
    
    mu_assert("Expression value should be 'post.name.name.name.name'", strcmp(ast->value, "post.name.name.name.name") == 0);

    mu_assert("No additional nodes expected", ast->next == NULL);

    FreeNodes(ast);
    return NULL;
}

const char* test_include() {
    Token tokens[] = {
        {TOKEN_STATEMENT_OPEN, "{%"},
        {TOKEN_INCLUDE, "include"},
        {TOKEN_QUOTE_OPEN, "\""},
        {TOKEN_STRING_LITERAL, "filename.html"},
        {TOKEN_QUOTE_CLOSE, "\""},
        {TOKEN_STATEMENT_CLOSE, "%"},
        {TOKEN_EOF, NULL}
    };

    int tokenCount = sizeof(tokens) / sizeof(tokens[0]);
    Node* ast = ParseTemplate(tokens, tokenCount);
    mu_assert("AST is NULL", ast != NULL);
    mu_assert("AST should be INCLUDE_NODE", ast->type == INCLUDE_NODE);
    
    mu_assert("Expression value should be 'filename.html'", strcmp(ast->value, "filename.html") == 0);

    mu_assert("No additional nodes expected", ast->next == NULL);

    FreeNodes(ast);
    return NULL;
}


const char *test_parser_all() {
    mu_run_test(test_parse_simple_template);
    mu_run_test(test_parse_empty_template);
    mu_run_test(test_parse_if_else_template);
    mu_run_test(test_parse_dot_notation_expression);
    mu_run_test(test_include);
    return 0;
}



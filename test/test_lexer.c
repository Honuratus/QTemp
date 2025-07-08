#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "minunit.h"


void free_token_str(Token t) {
    if (t.str) free(t.str);
}

static const char* test_basic_tokens() {
    const char* input = "Hello {{name}}!";
    Lexer* lexer = CreateLexer(input);
    mu_assert("Failed to create lexer", lexer != NULL);

    // TOKEN_TEXT: "Hello "
    Token t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_TEXT", t.type == TOKEN_TEXT);
    mu_assert("Token text incorrect", strcmp(t.str, "Hello ") == 0);
    free_token_str(t);

    // TOKEN_VAR_OPEN: "{{"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_VAR_OPEN", t.type == TOKEN_VAR_OPEN);
    mu_assert("Token var_open incorrect", strcmp(t.str, "{{") == 0);
    free_token_str(t);

    // TOKEN_IDENTIFIER: "name"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_IDENTIFIER", t.type == TOKEN_IDENTIFIER);
    mu_assert("Token identifier incorrect", strcmp(t.str, "name") == 0);
    free_token_str(t);

    // TOKEN_VAR_CLOSE: "}}"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_VAR_CLOSE", t.type == TOKEN_VAR_CLOSE);
    mu_assert("Token var_close incorrect", strcmp(t.str, "}}") == 0);
    free_token_str(t);

    // TOKEN_TEXT: "!"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_TEXT", t.type == TOKEN_TEXT);
    mu_assert("Token text incorrect", strcmp(t.str, "!") == 0);
    free_token_str(t);

    // TOKEN_EOF
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_EOF", t.type == TOKEN_EOF);
    mu_assert("EOF token str should be NULL", t.str == NULL);

    FreeLexer(lexer);

    return 0;
}

static const char* test_if_statement_token() {
    const char* input = "Hello {% if name %} Onur {% endif %}";
    Lexer* lexer = CreateLexer(input);
    mu_assert("Failed to create lexer", lexer != NULL);

    // TOKEN_TEXT: "Hello "
    Token t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_TEXT", t.type == TOKEN_TEXT);
    mu_assert("Token text incorrect | Hello ", strcmp(t.str, "Hello ") == 0);
    free_token_str(t);

    // TOKEN_STATEMENT_OPEN: "{%"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_OPEN", t.type == TOKEN_STATEMENT_OPEN);
    mu_assert("Token statement_open incorrect", strcmp(t.str, "{%") == 0);
    free_token_str(t);

    // TOKEN_IF: "if"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_IF", t.type == TOKEN_IF);
    mu_assert("Token text incorrect | if", strcmp(t.str, "if") == 0);
    free_token_str(t);

    // TOKEN_IDENTIFIER: "name"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_IDENTIFIER", t.type == TOKEN_IDENTIFIER);
    mu_assert("Token text incorrect | name", strcmp(t.str, "name") == 0);
    free_token_str(t);

    // TOKEN_STATEMENT_CLOSE: "%}"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_CLOSE", t.type == TOKEN_STATEMENT_CLOSE);
    mu_assert("Token text incorrect | %}", strcmp(t.str, "%}") == 0);
    free_token_str(t);

    // TOKEN_TEXT: " Onur "
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_TEXT", t.type == TOKEN_TEXT);
    mu_assert("Token text incorrect |  Onur ", strcmp(t.str, " Onur ") == 0);
    free_token_str(t);

    // TOKEN_STATEMENT_OPEN: "{%"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_OPEN", t.type == TOKEN_STATEMENT_OPEN);
    mu_assert("Token statement_open incorrect", strcmp(t.str, "{%") == 0);
    free_token_str(t);

    // TOKEN_ENDIF: "endif"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_ENDIF", t.type == TOKEN_ENDIF);
    mu_assert("Token text incorrect | endif", strcmp(t.str, "endif") == 0);
    free_token_str(t);

    // TOKEN_STATEMENT_CLOSE: "%}"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_CLOSE", t.type == TOKEN_STATEMENT_CLOSE);
    mu_assert("Token text incorrect | %}", strcmp(t.str, "%}") == 0);
    free_token_str(t);

    // TOKEN_EOF
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_EOF", t.type == TOKEN_EOF);
    mu_assert("EOF token str should be NULL", t.str == NULL);

    FreeLexer(lexer);

    return 0;
}

static const char* test_if_else_statement_token() {
    const char* input = "Hello {% if name %} Onur {% else %} Baran {% endif %}";
    Lexer* lexer = CreateLexer(input);
    mu_assert("Failed to create lexer", lexer != NULL);

    // TOKEN_TEXT: "Hello "
    Token t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_TEXT", t.type == TOKEN_TEXT);
    mu_assert("Token text incorrect | Hello ", strcmp(t.str, "Hello ") == 0);
    free_token_str(t);

    // TOKEN_STATEMENT_OPEN: "{%"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_OPEN", t.type == TOKEN_STATEMENT_OPEN);
    mu_assert("Token statement_open incorrect", strcmp(t.str, "{%") == 0);
    free_token_str(t);

    // TOKEN_IF: "if"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_IF", t.type == TOKEN_IF);
    mu_assert("Token text incorrect | if", strcmp(t.str, "if") == 0);
    free_token_str(t);

    // TOKEN_IDENTIFIER: "name"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_IDENTIFIER", t.type == TOKEN_IDENTIFIER);
    mu_assert("Token text incorrect | name", strcmp(t.str, "name") == 0);
    free_token_str(t);

    // TOKEN_STATEMENT_CLOSE: "%}"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_CLOSE", t.type == TOKEN_STATEMENT_CLOSE);
    mu_assert("Token text incorrect | %}", strcmp(t.str, "%}") == 0);
    free_token_str(t);

    // TOKEN_TEXT: " Onur "
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_TEXT", t.type == TOKEN_TEXT);
    mu_assert("Token text incorrect |  Onur ", strcmp(t.str, " Onur ") == 0);
    free_token_str(t);

    // TOKEN_STATEMENT_OPEN: "{%"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_OPEN", t.type == TOKEN_STATEMENT_OPEN);
    mu_assert("Token statement_open incorrect", strcmp(t.str, "{%") == 0);
    free_token_str(t);

    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_ELSE", t.type == TOKEN_ELSE);
    mu_assert("Token statement_open incorrect", strcmp(t.str, "else") == 0);
    free_token_str(t);
    

    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_CLOSE", t.type == TOKEN_STATEMENT_CLOSE);
    mu_assert("Token statement_open incorrect", strcmp(t.str, "%}") == 0);
    free_token_str(t);

    
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_TEXT", t.type == TOKEN_TEXT);
    mu_assert("Token statement_open incorrect", strcmp(t.str, " Baran ") == 0);
    free_token_str(t);



    // TOKEN_STATEMENT_OPEN: "{%"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_OPEN", t.type == TOKEN_STATEMENT_OPEN);
    mu_assert("Token statement_open incorrect", strcmp(t.str, "{%") == 0);
    free_token_str(t);

    // TOKEN_ENDIF: "endif"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_ENDIF", t.type == TOKEN_ENDIF);
    mu_assert("Token text incorrect | endif", strcmp(t.str, "endif") == 0);
    free_token_str(t);

    // TOKEN_STATEMENT_CLOSE: "%}"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_CLOSE", t.type == TOKEN_STATEMENT_CLOSE);
    mu_assert("Token text incorrect | %}", strcmp(t.str, "%}") == 0);
    free_token_str(t);

    // TOKEN_EOF
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_EOF", t.type == TOKEN_EOF);
    mu_assert("EOF token str should be NULL", t.str == NULL);

    FreeLexer(lexer);

    return 0;
}

static const char* test_for_statement_token() {
    const char* input = "List: {% for item in items %} {{ item }} {% endfor %}";
    Lexer* lexer = CreateLexer(input);
    mu_assert("Failed to create lexer", lexer != NULL);

    // TOKEN_TEXT: "List: "
    Token t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_TEXT", t.type == TOKEN_TEXT);
    mu_assert("Token text incorrect | List: ", strcmp(t.str, "List: ") == 0);
    free_token_str(t);

    // TOKEN_STATEMENT_OPEN: "{%"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_OPEN", t.type == TOKEN_STATEMENT_OPEN);
    mu_assert("Token statement_open incorrect", strcmp(t.str, "{%") == 0);
    free_token_str(t);

    // TOKEN_FOR: "for"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_FOR", t.type == TOKEN_FOR);
    mu_assert("Token text incorrect | for", strcmp(t.str, "for") == 0);
    free_token_str(t);

    // TOKEN_IDENTIFIER: "item"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_IDENTIFIER", t.type == TOKEN_IDENTIFIER);
    mu_assert("Token text incorrect | item", strcmp(t.str, "item") == 0);
    free_token_str(t);

    // TOKEN_IN: "in"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_IN", t.type == TOKEN_IN);
    mu_assert("Token text incorrect | in", strcmp(t.str, "in") == 0);
    free_token_str(t);

    // TOKEN_IDENTIFIER: "items"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_IDENTIFIER", t.type == TOKEN_IDENTIFIER);
    mu_assert("Token text incorrect | items", strcmp(t.str, "items") == 0);
    free_token_str(t);

    // TOKEN_STATEMENT_CLOSE: "%}"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_CLOSE", t.type == TOKEN_STATEMENT_CLOSE);
    mu_assert("Token text incorrect | %}", strcmp(t.str, "%}") == 0);
    free_token_str(t);

    // TOKEN_TEXT: " "
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_TEXT", t.type == TOKEN_TEXT);
    mu_assert("Token text incorrect |  ", strcmp(t.str, " ") == 0);
    free_token_str(t);

    // TOKEN_VAR_OPEN: "{{"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_VAR_OPEN", t.type == TOKEN_VAR_OPEN);
    mu_assert("Token text incorrect | {{", strcmp(t.str, "{{") == 0);
    free_token_str(t);

    // TOKEN_IDENTIFIER: "item"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_IDENTIFIER", t.type == TOKEN_IDENTIFIER);
    mu_assert("Token text incorrect | item", strcmp(t.str, "item") == 0);
    free_token_str(t);

    // TOKEN_VARL_CLOSE: "}}"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_VAR_CLOSE", t.type == TOKEN_VAR_CLOSE);
    mu_assert("Token text incorrect | }}", strcmp(t.str, "}}") == 0);
    free_token_str(t);

    // TOKEN_TEXT: " "
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_TEXT", t.type == TOKEN_TEXT);
    mu_assert("Token text incorrect |  ", strcmp(t.str, " ") == 0);
    free_token_str(t);

    // TOKEN_STATEMENT_OPEN: "{%"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_OPEN", t.type == TOKEN_STATEMENT_OPEN);
    mu_assert("Token text incorrect | {%", strcmp(t.str, "{%") == 0);
    free_token_str(t);

    // TOKEN_ENDFOR: "endfor"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_ENDFOR", t.type == TOKEN_ENDFOR);
    mu_assert("Token text incorrect | endfor", strcmp(t.str, "endfor") == 0);
    free_token_str(t);

    // TOKEN_STATEMENT_CLOSE: "%}"
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_CLOSE", t.type == TOKEN_STATEMENT_CLOSE);
    mu_assert("Token text incorrect | %}", strcmp(t.str, "%}") == 0);
    free_token_str(t);

    // TOKEN_EOF
    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_EOF", t.type == TOKEN_EOF);
    mu_assert("EOF token str should be NULL", t.str == NULL);

    FreeLexer(lexer);
    return 0;
}

const char* test_lexer_var_accses(){
    const char* input = "{{ post.data }}";
    Lexer* lexer = CreateLexer(input);
    mu_assert("Failed to create lexer", lexer != NULL);

    Token t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_VAR_OPEN", t.type == TOKEN_VAR_OPEN);
    mu_assert("Token text incorrect | {{", strcmp(t.str, "{{") == 0);
    free_token_str(t);

    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_IDENTIFIER", t.type == TOKEN_IDENTIFIER);
    mu_assert("Token text incorrect | post", strcmp(t.str, "post") == 0);
    free_token_str(t);

    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_DOT", t.type == TOKEN_DOT);
    mu_assert("Token text incorrect | .", strcmp(t.str, ".") == 0);
    free_token_str(t);

    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_IDENTIFIER", t.type == TOKEN_IDENTIFIER);
    mu_assert("Token text incorrect | data", strcmp(t.str, "data") == 0);
    free_token_str(t);

    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_VAR_CLOSE", t.type == TOKEN_VAR_CLOSE);
    mu_assert("Token text incorrect | }}", strcmp(t.str, "}}") == 0);
    free_token_str(t);

    FreeLexer(lexer);
    return 0;

}

const char* test_lexer_include_statement(){
    const char* input = "{% include \"header.html\" %}";
    Lexer* lexer = CreateLexer(input);
    mu_assert("Failed to create lexer", lexer != NULL);

    Token t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_OPEN", t.type == TOKEN_STATEMENT_OPEN);
    mu_assert("Token text incorrect | {%", strcmp(t.str, "{%") == 0);
    free_token_str(t);

    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_IDENTIFIER", t.type == TOKEN_IDENTIFIER);
    mu_assert("Token text incorrect | include", strcmp(t.str, "include") == 0);
    free_token_str(t);

    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_QUOTE_OPEN", t.type == TOKEN_QUOTE_OPEN);
    mu_assert("Token text incorrect | \"", strcmp(t.str, "\"") == 0);
    free_token_str(t);

    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STRING_LITERAL", t.type == TOKEN_STRING_LITERAL);
    mu_assert("Token text incorrect | header.html", strcmp(t.str, "header.html") == 0);
    free_token_str(t);

    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_QUOTE_CLOSE", t.type == TOKEN_QUOTE_CLOSE);
    mu_assert("Token text incorrect | \"", strcmp(t.str, "\"") == 0);
    free_token_str(t);


    t = LexerNextToken(lexer);
    mu_assert("Token type not TOKEN_STATEMENT_CLOSE", t.type == TOKEN_STATEMENT_CLOSE);
    mu_assert("Token text incorrect | %}", strcmp(t.str, "%}") == 0);
    free_token_str(t);

    FreeLexer(lexer);
    return 0;
}



const char *test_lexer_all() {
    mu_run_test(test_basic_tokens);
    mu_run_test(test_if_statement_token);
    mu_run_test(test_for_statement_token);
    mu_run_test(test_if_else_statement_token);
    mu_run_test(test_lexer_var_accses);
    mu_run_test(test_lexer_include_statement);
    return 0;
}

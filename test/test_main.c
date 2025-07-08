#include <stdio.h>
#include "minunit.h"

extern const char *test_dict_all();
extern const char *test_lexer_all();
extern const char *test_parser_all();
extern const char *test_engine_all();

int tests_run = 0;

int main() {
    const char *result;

    result = test_dict_all();
    if (result != 0) {
        printf("❌ Dictionary tests failed: %s\n", result);
        return 1;
    }
    else{
        printf("✅ Dictionary tests passed\n");
    }

    result = test_lexer_all();
    if (result != 0) {
        printf("❌ Lexer tests failed: %s\n", result);
        return 1;
    }
    else{
        printf("✅ Lexer tests passed\n");
    }

    result = test_parser_all();
    if(result != 0){
        printf("❌ Parser tests failed: %s\n", result);
        return 1;
    }
    else{
        printf("✅ Parser tests passed\n");

    }

    result = test_engine_all();
    if (result != 0) {
        printf("❌ Engine tests failed: %s\n", result);
        return 1;
    }
    else{
        printf("✅ Engine tests passed\n");
    }

    printf("✅ All tests passed\n");
    printf("Tests run: %d\n", tests_run);
    return 0;
}

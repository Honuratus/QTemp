# QTemp

A minimalist and embeddable template engine designed for the C programming language.

## Features

- **If-Else**: Display content conditionally  
- **Loops**: Iterate over lists and dicts using `for` loops  
- **Dict Access**: Access dictionary values and object-like structures in templates  
- **Include**: Include other templates within a template  

## Usage

    1. Prepare your context using dictionaries and containers  
    2. Define your template as a string (or load from file)  
    3. Call `render()` to generate the output  



## Example
```c
#include <stdio.h>
#include <stdlib.h>
#include "engine.h"
#include "dict.h"

int main() {
    // Create user dict: { "name": "John" }
    Dictionary* user = CreateDictionary(1);
    DictionaryAddString(user, "name", "John");

    // Create list: [1, 2, 3]
    Container items = CreateContainer(3,
        ValueFromInt(1),
        ValueFromInt(2),
        ValueFromInt(3)
    );

    // Create context
    Dictionary* ctx = CreateDictionary(2);
    DictionaryAddDict(ctx, "user", user);
    DictionaryAddContainer(ctx, "items", items);

    // Template string
    const char* tpl =
        "Hi {{ user.name }}!\n"
        "{% for item in items %}"
        "- {{ item }}\n"
        "{% endfor %}";

    // Render
    char* out = render(tpl, ctx);
    printf("%s", out);

    // Clean up
    free(out);
    FreeDictionary(ctx);
    return 0;
}

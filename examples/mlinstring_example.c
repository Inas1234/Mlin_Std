#include "../include/mlinstd.h"

int main(){
    // Create an empty MlinString
    MlinString *str1 = mlin_string_create();
    printf("str1: '%s'\n", str1->contets);

    // Create MlinString with a source string
    MlinString *str2 = mlin_string_create_with_src("Hello, world!");
    printf("str2: '%s'\n", str2->contets);

    // Append to str1
    mlin_string_append(str1, "Hello");
    printf("str1 after append: '%s'\n", str1->contets);

    // Insert into str1
    mlin_string_insert(str1, ", my friend", 5);
    printf("str1 after insert: '%s'\n", str1->contets);

    // Substring of str2
    MlinString *substr = mlin_string_substring(str2, 0, 5);
    printf("Substring of str2: '%s'\n", substr->contets);

    // Trim example (add spaces to test)
    MlinString *str3 = mlin_string_create_with_src("  Trim this  ");
    mlin_string_trim(str3);
    printf("str3 after trim: '%s'\n", str3->contets);

    // Replace a substring in str2
    mlin_string_replace(str2, "world", "C programmer");
    printf("str2 after replace: '%s'\n", str2->contets);

    // Convert str2 to uppercase
    mlin_string_to_upper(str2);
    printf("str2 after to_upper: '%s'\n", str2->contets);

    // Convert str2 to lowercase
    mlin_string_to_lower(str2);
    printf("str2 after to_lower: '%s'\n", str2->contets);

    // Split str2 into words
    size_t count = 0;
    MlinString **split = mlin_string_split(str2, " ", &count);
    printf("str2 split into %zu parts:\n", count);
    for (size_t i = 0; i < count; i++) {
        printf("split[%zu]: '%s'\n", i, split[i]->contets);
        mlin_string_free(split[i]);
    }
    free(split);

    // Get character at index in str2
    printf("Character at index 2 in str2: '%c'\n", mlin_string_char_at(str2, 2));

    // Clear str2
    mlin_string_clear(str2);
    printf("str2 after clear: '%s'\n", str2->contets);

    // Clean up memory
    mlin_string_free(str1);
    mlin_string_free(str2);
    mlin_string_free(substr);
    mlin_string_free(str3);

    return 0;
}
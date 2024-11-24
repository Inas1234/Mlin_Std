#include "../include/mlinstd.h"

int main() {
    // -----------------------------
    // Example 1: Dynamic Array of Integers
    // -----------------------------
    printf("Dynamic Array of Integers:\n");
    MlinArray *int_array = mlin_array_create(sizeof(int));

    // Append integers
    for (int i = 1; i <= 5; i++) {
        MLIN_APPEND(int_array, i);
    }

    // Insert an integer at index 2
    int value_to_insert = 42;
    mlin_array_insert(int_array, 2, &value_to_insert);

    // Find an integer in the array
    int target_int = 42;
    size_t int_index = mlin_array_find(int_array, &target_int, mlin_cmp_int);
    if (int_index != (size_t)-1) {
        printf("Found integer %d at index %zu\n", target_int, int_index);
    } else {
        printf("Integer %d not found\n", target_int);
    }

    // Remove an integer at index 3
    mlin_array_remove(int_array, 3);

    // Print all integers
    printf("Final integer array contents:\n");
    for (size_t i = 0; i < int_array->count; i++) {
        printf("int_array[%zu] = %d\n", i, MLIN_GET(int_array, int, i));
    }

    mlin_array_free(int_array);

    // -----------------------------
    // Example 2: Dynamic Array of MlinString
    // -----------------------------
    printf("\nDynamic Array of MlinString:\n");
    MlinArray *str_array = mlin_array_create(sizeof(MlinString *));

    // Create and append MlinStrings
    MlinString *str1 = mlin_string_create_with_src("Alice");
    MlinString *str2 = mlin_string_create_with_src("Bob");
    MlinString *str3 = mlin_string_create_with_src("Charlie");
    MlinString *str4 = mlin_string_create_with_src("David");

    MLIN_APPEND(str_array, str1);
    MLIN_APPEND(str_array, str2);
    MLIN_APPEND(str_array, str3);
    MLIN_APPEND(str_array, str4);

    // Insert a new string
    MlinString *inserted_str = mlin_string_create_with_src("Eve");
    mlin_array_insert(str_array, 2, &inserted_str);

    // Find a string
    MlinString *target_str = mlin_string_create_with_src("Charlie");
    size_t str_index = mlin_array_find(str_array, &target_str, mlin_cmp_mlinstring);
    if (str_index != (size_t)-1) {
        printf("Found string '%s' at index %zu\n", target_str->contets, str_index);
    } else {
        printf("String '%s' not found\n", target_str->contets);
    }
    mlin_string_free(target_str);

    // Remove a string at index 1
    mlin_array_remove(str_array, 1);

    // Print all strings
    printf("Final string array contents:\n");
    for (size_t i = 0; i < str_array->count; i++) {
        MlinString *str = MLIN_GET(str_array, MlinString *, i);
        printf("str_array[%zu] = %s\n", i, str->contets);
    }

    // Free all strings and the array
    for (size_t i = 0; i < str_array->count; i++) {
        MlinString *str = MLIN_GET(str_array, MlinString *, i);
        mlin_string_free(str);
    }
    mlin_array_free(str_array);

    return 0;
}

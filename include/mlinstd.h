#ifndef MLIN_STD
#define MLIN_STD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>

typedef struct
{
    char * contets;
    size_t size;
    size_t capacity;
} MlinString;

typedef struct {
    void * items;
    size_t count;
    size_t capacity;
    size_t elem_size;
} MlinArray;

#define MLIN_INIT_CAP 16

#define MLIN_LOG(msg) \
    printf("[LOG] %s:%d in %s: %s\n", __FILE__, __LINE__, __func__, msg)

#define MLIN_ERROR(msg) \
    fprintf(stderr, "[ERROR] %s in function %s at line %d: %s\n", msg, __func__, __LINE__, strerror(errno))

#define MLIN_APPEND(da, item)                                                       \
    do {                                                                               \
        if ((da)->count >= (da)->capacity) {                                           \
            size_t new_capacity = (da)->capacity == 0 ? MLIN_INIT_CAP : (da)->capacity * 2; \
            void *new_items = realloc((da)->items, new_capacity * (da)->elem_size);    \
            if (!new_items) {                                                         \
                MLIN_ERROR("Failed to allocate memory for dynamic array");                \
                exit(EXIT_FAILURE);                                                   \
            }                                                                         \
            (da)->items = new_items;                                                  \
            (da)->capacity = new_capacity;                                            \
        }                                                                             \
        memcpy((char *)(da)->items + ((da)->count * (da)->elem_size), &(item), (da)->elem_size); \
        (da)->count++;                                                                \
    } while (0)


#define MLIN_GET(da, type, index) (*((type *)mlin_array_get(da, index)))

#define MLIN_STR_FREE(da) \
    for (size_t i = 0; i < da->count; i++){ \
        MlinString *str = MLIN_GET(da, MlinString *, i); \
        mlin_string_free(str); \
    }\
    mlin_array_free(da);


//MLIN ARRAY
MlinArray *mlin_array_create(size_t elem_size);
void mlin_array_free(MlinArray *arr);
void *mlin_array_get(MlinArray *arr, size_t index);
void mlin_array_set(MlinArray *arr, size_t index, const void *value);
void mlin_array_clear(MlinArray *arr);
void mlin_array_insert(MlinArray *arr, size_t index, const void *value);
void mlin_array_remove(MlinArray *arr, size_t index);
size_t mlin_array_find(MlinArray *arr, const void *value, int (*cmp)(const void *, const void *));
void mlin_array_resize(MlinArray *arr, size_t new_capacity);
int mlin_cmp_int(const void *a, const void *b);
int mlin_cmp_float(const void *a, const void *b);
int mlin_cmp_str(const void *a, const void *b);
int mlin_cmp_mlinstring(const void *a, const void *b);

// MLIN STRING
MlinString * mlin_string_create_with_src(const char * src);
MlinString * mlin_string_create();
void mlin_string_free(MlinString * mlinstring);
void string_expand(MlinString *str, size_t min_capacity);
bool mlin_string_cmp(MlinString * str1, MlinString * str2);
void mlin_string_append(MlinString *str, const char *suffix);
void mlin_string_insert(MlinString *str, const char *substr, size_t pos);
MlinString *mlin_string_substring(MlinString *str, size_t start, size_t length);
void mlin_string_trim(MlinString *str);
void mlin_string_replace(MlinString *str, const char *old_sub, const char *new_sub);
void mlin_string_to_upper(MlinString *str);
void mlin_string_to_lower(MlinString *str);
MlinString **mlin_string_split(MlinString *str, const char *delim, size_t *count);
void mlin_string_clear(MlinString *str);
char mlin_string_char_at(MlinString *str, size_t index);


// MLIN FILE I/O UTILITES
MlinString *mlin_file_to_mlinstring(const char *filename);
char *mlin_file_to_char(const char *filename);
bool mlin_string_to_file(const char *filename, MlinString *str);
bool mlin_char_to_file(const char *filename, const char *contents);


// MLIN FILE I/O UTILITES IMPLEMENTATION
// File Read and Write Functions
MlinString *mlin_file_to_mlinstring(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        MLIN_ERROR("Failed to open file for reading");
        return NULL;
    }

    MlinString *result = mlin_string_create();

    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        buffer[bytesRead] = '\0';
        mlin_string_append(result, buffer);
    }

    if (ferror(file)) {
        MLIN_ERROR("Error while reading file");
        mlin_string_free(result);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return result;
}

char *mlin_file_to_char(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        MLIN_ERROR("Failed to open file for reading");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    rewind(file);

    char *contents = (char *)malloc(filesize + 1);
    if (!contents) {
        MLIN_ERROR("Failed to allocate memory for file contents");
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(contents, 1, filesize, file);
    if (bytesRead < filesize) {
        MLIN_ERROR("Error while reading file");
        free(contents);
        fclose(file);
        return NULL;
    }

    contents[filesize] = '\0';
    fclose(file);
    return contents;
}

bool mlin_string_to_file(const char *filename, MlinString *str) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        MLIN_ERROR("Failed to open file for writing");
        return false;
    }

    if (fwrite(str->contets, 1, str->size, file) < str->size) {
        MLIN_ERROR("Failed to write data to file");
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}

bool mlin_char_to_file(const char *filename, const char *contents) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        MLIN_ERROR("Failed to open file for writing");
        return false;
    }

    size_t length = strlen(contents);
    if (fwrite(contents, 1, length, file) < length) {
        MLIN_ERROR("Failed to write data to file");
        fclose(file);
        return false;
    }

    fclose(file);
    return true;
}



//MLIN ARRAY IMPLEMENTATION
MlinArray *mlin_array_create(size_t elem_size) {
    MlinArray * arr = (MlinArray *)malloc(sizeof(MlinArray));
    if (!arr){
        MLIN_ERROR("FAILED TO ALLOCATE MEMORY");
        exit(EXIT_FAILURE);
    }
    arr->count = 0;
    arr->capacity = 0;
    arr->elem_size = elem_size;
    arr->items = NULL;

    return arr;
}

void mlin_array_free(MlinArray *arr){
    free(arr->items);
    free(arr);
}

void *mlin_array_get(MlinArray *arr, size_t index) {
    if (index >= arr->count) {
        fprintf(stderr, "[ERROR] Index %zu out of bounds for dynamic array of size %zu\n", index, arr->count);
        exit(EXIT_FAILURE);
    }
    return (char *)arr->items + (index * arr->elem_size);
}

void mlin_array_set(MlinArray *arr, size_t index, const void *value) {
    if (index >= arr->count) {
        fprintf(stderr, "[ERROR] Index %zu out of bounds for dynamic array of size %zu\n", index, arr->count);
        exit(EXIT_FAILURE);
    }
    memcpy((char *)arr->items + (index * arr->elem_size), value, arr->elem_size);
}

void mlin_array_clear(MlinArray *arr) {
    arr->count = 0;
}

void mlin_array_insert(MlinArray *arr, size_t index, const void *value) {
    if (index > arr->count) {
        fprintf(stderr, "[ERROR] Index %zu out of bounds for dynamic array of size %zu\n", index, arr->count);
        exit(EXIT_FAILURE);
    }
    if (arr->count >= arr->capacity) {
        size_t new_capacity = arr->capacity == 0 ? MLIN_INIT_CAP : arr->capacity * 2;
        arr->items = realloc(arr->items, new_capacity * arr->elem_size);
        if (!arr->items) {
            perror("Failed to allocate memory for dynamic array");
            exit(EXIT_FAILURE);
        }
        arr->capacity = new_capacity;
    }
    memmove((char *)arr->items + ((index + 1) * arr->elem_size),
            (char *)arr->items + (index * arr->elem_size),
            (arr->count - index) * arr->elem_size);
    memcpy((char *)arr->items + (index * arr->elem_size), value, arr->elem_size);
    arr->count++;
}

void mlin_array_remove(MlinArray *arr, size_t index) {
    if (index >= arr->count) {
        fprintf(stderr, "[ERROR] Index %zu out of bounds for dynamic array of size %zu\n", index, arr->count);
        exit(EXIT_FAILURE);
    }
    memmove((char *)arr->items + (index * arr->elem_size),
            (char *)arr->items + ((index + 1) * arr->elem_size),
            (arr->count - index - 1) * arr->elem_size);
    arr->count--;
}

size_t mlin_array_find(MlinArray *arr, const void *value, int (*cmp)(const void *, const void *)) {
    for (size_t i = 0; i < arr->count; i++) {
        if (cmp(value, (char *)arr->items + (i * arr->elem_size)) == 0) {
            return i;
        }
    }
    return (size_t)-1; 
}

void mlin_array_resize(MlinArray *arr, size_t new_capacity) {
    if (new_capacity < arr->count) {
        fprintf(stderr, "[ERROR] New capacity %zu cannot be smaller than current size %zu\n", new_capacity, arr->count);
        exit(EXIT_FAILURE);
    }
    void *new_items = realloc(arr->items, new_capacity * arr->elem_size);
    if (!new_items) {
        perror("Failed to allocate memory for dynamic array");
        exit(EXIT_FAILURE);
    }
    arr->items = new_items;
    arr->capacity = new_capacity;
}

int mlin_cmp_int(const void *a, const void *b) {
    int int_a = *(const int *)a;
    int int_b = *(const int *)b;
    return (int_a > int_b) - (int_a < int_b); 
}

int mlin_cmp_float(const void *a, const void *b) {
    float float_a = *(const float *)a;
    float float_b = *(const float *)b;
    return (float_a > float_b) - (float_a < float_b); 
}

int mlin_cmp_str(const void *a, const void *b) {
    const char *str_a = *(const char **)a;
    const char *str_b = *(const char **)b;
    return strcmp(str_a, str_b);
}

int mlin_cmp_mlinstring(const void *a, const void *b) {
    const MlinString *str_a = *(const MlinString **)a;
    const MlinString *str_b = *(const MlinString **)b;
    return strcmp(str_a->contets, str_b->contets);
}


//MLIN STRING IMPLEMENTATION
void string_expand(MlinString *str, size_t min_capacity) {
    if (min_capacity <= str->capacity) return;
    size_t new_capacity = str->capacity * 2;
    if (new_capacity < min_capacity) {
        new_capacity = min_capacity;
    }
    char *new_data = (char *)realloc(str->contets, new_capacity * sizeof(char));
    if (!new_data) {
        MLIN_ERROR("Failed to reallocate memory");
        exit(EXIT_FAILURE);
    }
    str->contets = new_data;
    str->capacity = new_capacity;
}

MlinString * mlin_string_create_with_src(const char * src) {
    MlinString * newstring = (MlinString*)malloc(sizeof(MlinString));
    newstring->capacity = 16;
    newstring->size = 0;
    newstring->contets = (char *)malloc(newstring->capacity * sizeof(char));
    if (!newstring->contets) {
        MLIN_ERROR("Failed to allocate memeory");
        exit(EXIT_FAILURE);
    }
    newstring->contets[0] = '\0';


    size_t src_length = strlen(src);
    if (newstring->size + src_length >= newstring->capacity) {
        string_expand(newstring, newstring->size + src_length + 1);
    }

    memcpy(newstring->contets + newstring->size, src, src_length);
    newstring->size += src_length;
    newstring->contets[newstring->size] = '\0';

    return newstring;
}

MlinString * mlin_string_create() {
        MlinString * newstring = (MlinString*)malloc(sizeof(MlinString));
    newstring->capacity = 16;
    newstring->size = 0;
    newstring->contets = (char *)malloc(newstring->capacity * sizeof(char));
    if (!newstring->contets) {
        MLIN_ERROR("Failed to allocate memeory");
        exit(EXIT_FAILURE);
    }
    newstring->contets[0] = '\0';

    return newstring;
}

void mlin_string_free(MlinString * mlinstring){
    free(mlinstring->contets);
    mlinstring->contets = NULL;
    mlinstring->capacity = 0;
    mlinstring->size = 0;
    free(mlinstring);
}

bool mlin_string_cmp(MlinString * str1, MlinString * str2) {
    int result = strcmp(str1->contets, str2->contets);
    if (result == 0){
        return true;
    }
    else {
        return false;
    }

    return false;
}


void mlin_string_append(MlinString *str, const char *suffix) {
    size_t suffix_len = strlen(suffix);
    if (str->size + suffix_len >= str->capacity) {
        string_expand(str, str->size + suffix_len + 1);
    }
    memcpy(str->contets + str->size, suffix, suffix_len);
    str->size += suffix_len;
    str->contets[str->size] = '\0';
}

void mlin_string_insert(MlinString *str, const char *substr, size_t pos) {
    if (pos > str->size) {
        MLIN_ERROR("Insert position is out of bounds");
        exit(EXIT_FAILURE);
    }
    size_t substr_len = strlen(substr);
    if (str->size + substr_len >= str->capacity) {
        string_expand(str, str->size + substr_len + 1);
    }
    memmove(str->contets + pos + substr_len, str->contets + pos, str->size - pos + 1);
    memcpy(str->contets + pos, substr, substr_len);
    str->size += substr_len;
}


MlinString *mlin_string_substring(MlinString *str, size_t start, size_t length) {
    if (start >= str->size) {
        MLIN_ERROR("Start position is out of bounds");
        exit(EXIT_FAILURE);
    }
    if (start + length > str->size) {
        length = str->size - start;  // Clamp length to the end of the string
    }
    MlinString *substring = mlin_string_create_with_src("");
    string_expand(substring, length + 1);
    memcpy(substring->contets, str->contets + start, length);
    substring->size = length;
    substring->contets[length] = '\0';
    return substring;
}


void mlin_string_trim(MlinString *str) {
    size_t start = 0;
    while (start < str->size && isspace(str->contets[start])) {
        start++;
    }

    size_t end = str->size;
    while (end > start && isspace(str->contets[end - 1])) {
        end--;
    }

    size_t new_length = end - start;
    memmove(str->contets, str->contets + start, new_length);
    str->size = new_length;
    str->contets[new_length] = '\0';
}

void mlin_string_replace(MlinString *str, const char *old_sub, const char *new_sub) {
    size_t old_len = strlen(old_sub);
    size_t new_len = strlen(new_sub);

    char *pos = strstr(str->contets, old_sub);
    while (pos) {
        size_t offset = pos - str->contets;

        if (old_len != new_len) {
            if (str->size + new_len - old_len >= str->capacity) {
                string_expand(str, str->size + new_len - old_len + 1);
            }
            memmove(str->contets + offset + new_len, str->contets + offset + old_len,
                    str->size - offset - old_len + 1);
        }

        memcpy(str->contets + offset, new_sub, new_len);
        str->size += new_len - old_len;

        pos = strstr(str->contets + offset + new_len, old_sub);
    }
}

void mlin_string_to_upper(MlinString *str) {
    for (size_t i = 0; i < str->size; i++) {
        str->contets[i] = toupper((unsigned char)str->contets[i]);
    }
}

void mlin_string_to_lower(MlinString *str) {
    for (size_t i = 0; i < str->size; i++) {
        str->contets[i] = tolower((unsigned char)str->contets[i]);
    }
}

MlinString **mlin_string_split(MlinString *str, const char *delim, size_t *count) {
    *count = 0;
    MlinString **result = NULL;

    char *token = strtok(str->contets, delim);
    while (token) {
        result = realloc(result, (*count + 1) * sizeof(MlinString *));
        result[*count] = mlin_string_create_with_src(token);
        (*count)++;
        token = strtok(NULL, delim);
    }

    return result;
}

void mlin_string_clear(MlinString *str) {
    str->size = 0;
    str->contets[0] = '\0';
}

char mlin_string_char_at(MlinString *str, size_t index) {
    if (index >= str->size) {
        MLIN_ERROR("Index out of bounds");
        exit(EXIT_FAILURE);
    }
    return str->contets[index];
}

#endif
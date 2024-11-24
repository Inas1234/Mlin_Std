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

#define MLIN_LOG(msg) \
    printf("[LOG] %s:%d in %s: %s\n", __FILE__, __LINE__, __func__, msg)

#define MLIN_ERROR(msg) \
    fprintf(stderr, "[ERROR] %s in function %s at line %d: %s\n", msg, __func__, __LINE__, strerror(errno))

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


void string_expand(MlinString *str, size_t min_capacity) {
    if (min_capacity <= str->capacity) return;
    size_t new_capacity = str->capacity * 2;
    if (new_capacity < min_capacity) {
        new_capacity = min_capacity;
    }
    char *new_data = (char *)realloc(str->contets, new_capacity * sizeof(char));
    if (!new_data) {
        perror("Failed to reallocate memory");
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
        perror("Failed to allocate memeory");
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
        perror("Insert position is out of bounds");
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
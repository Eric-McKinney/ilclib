#include <stdlib.h>

typedef struct {
    char *chars;
    size_t len;
} String;

static void mem_copy(void *dest, void *src, size_t size) {
    int i;
    for (i = 0; i < size; i++) {
        dest[i] = src[i];
    }
}

String *alloc_string(const char *chars, size_t len) {
    if (chars == NULL) {
        return NULL;
    }

    String *str = malloc(sizeof(String));
    str->chars = malloc(len * sizeof(char));

    mem_copy(str->chars, chars, len);

    return str;
}

String *string_copy(const String *str) {
    if (str == NULL) {
        return NULL;
    }

    return alloc_string(str->chars, str->len);
}

void free_string(String *str) {
    if (str == NULL) {
        return;
    }

    free(str->chars);
    free(str);
}

size_t string_length(const String *str) {
    return str == NULL ? 0 : str->len;
}

String *substring(const String *str, int start, int end) {
    if (str == NULL || start < str->len * -1 || end < str->len * -1) {
        return NULL;
    }

    if (start < 0) {
        start = str->len + start;
    }
    if (end < 0) {
        end = str->len + end;
    }

    int len = end - start;
    if (len < 0 || len >= str->len) {
        return NULL;
    }

    String *substr = malloc(sizeof(String));
    substr->chars = malloc(len * sizeof(char));
    substr->len = len;

    mem_copy(substr->chars, str->chars + start, len);

    return sub;
}


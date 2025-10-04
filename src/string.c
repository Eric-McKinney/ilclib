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

static void reverse_mem_copy(void *dest, void *src, size_t size) {
    int i_dest, i_src;
    for (i_dest = 0, i_src = size - 1; i_dest < size; i_dest++, i_src--) {
        dest[i_dest] = src[i_src];
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

String *string_reverse(const String *str) {
    if (str == NULL) {
        return NULL;
    }

    String *reverse = malloc(sizeof(String));
    reverse->chars = malloc(str->len * sizeof(char));
    reverse_mem_copy(reverse->chars, str->chars, str->len);
    reverse->len = str->len;

    return reverse;
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


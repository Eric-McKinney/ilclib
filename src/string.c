#include <stdlib.h>
#include <ilc/string.h>


static void mem_copy(void *dest, const void *src, size_t size) {
    unsigned int i;
    for (i = 0; i < size; i++) {
        ((char *)dest)[i] = ((char *)src)[i];
    }
}

static void reverse_mem_copy(void *dest, const void *src, size_t size) {
    unsigned int i_dest, i_src;
    for (i_dest = 0, i_src = size - 1; i_dest < size; i_dest++, i_src--) {
        ((char *)dest)[i_dest] = ((char *)src)[i_src];
    }
}

String *create_string(const char *chars, size_t len) {
    if (chars == NULL) {
        return NULL;
    }

    String *str = malloc(sizeof(String));
    str->chars = malloc(len * sizeof(char));
    str->len = len;

    mem_copy(str->chars, chars, len);

    return str;
}

String *string_copy(const String *str) {
    if (str == NULL) {
        return NULL;
    }

    return create_string(str->chars, str->len);
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
    if (str == NULL || (unsigned int) abs(start) >= str->len || (unsigned int) abs(end) > str->len) {
        return NULL;
    }

    if (start < 0) {
        start = str->len + start;
    }
    if (end < 0) {
        end = str->len + end;
    }

    int len = end - start;

    String *substr = malloc(sizeof(String));
    substr->chars = malloc(len * sizeof(char));
    substr->len = len;

    if (len > 0) {
        mem_copy(substr->chars, str->chars + start, len);
    } else if (len < 0) {
        reverse_mem_copy(substr->chars, str->chars + start, len);
    }

    return substr;
}


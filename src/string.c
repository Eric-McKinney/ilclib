#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ilc/string.h>


static void mem_copy(void *dest, const void *src, size_t size) {
    size_t i;
    for (i = 0; i < size; i++) {
        ((char *)dest)[i] = ((char *)src)[i];
    }
}

static void reverse_mem_copy(void *dest, const void *src, size_t size) {
    size_t i_dest, i_src;
    for (i_dest = 0, i_src = size - 1; i_dest < size; i_dest++, i_src--) {
        ((char *)dest)[i_dest] = ((char *)src)[i_src];
    }
}

String *create_string(const char *chars, size_t len) {
    if (chars == NULL) {
        errno = EFAULT;
        return NULL;
    }

    String *str = malloc(sizeof(String));

    if (str == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    str->chars = malloc(len * sizeof(char));

    if (str->chars == NULL) {
        errno = ENOMEM;
        free(str);
        return NULL;
    }

    str->len = len;
    mem_copy(str->chars, chars, len);

    return str;
}

String *string_copy(const String *str) {
    if (str == NULL) {
        errno = EFAULT;
        return NULL;
    }

    return create_string(str->chars, str->len);
}

String *string_reverse(const String *str) {
    if (str == NULL) {
        errno = EFAULT;
        return NULL;
    }

    String *reverse = malloc(sizeof(String));

    if (reverse == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    reverse->chars = malloc(str->len * sizeof(char));

    if (reverse->chars == NULL) {
        errno = ENOMEM;
        free(reverse);
        return NULL;
    }

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

String *substring(const String *str, int start, int end) {
    if (str == NULL) {
        errno = EFAULT;
        return NULL;
    }

    if ((unsigned int) abs(start) >= str->len || (unsigned int) abs(end) > str->len) {
        errno = EDOM;
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

    if (substr == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    substr->chars = malloc(len * sizeof(char));

    if (substr->chars == NULL) {
        errno = ENOMEM;
        free(substr);
        return NULL;
    }

    substr->len = len;

    if (len > 0) {
        mem_copy(substr->chars, str->chars + start, len);
    } else if (len < 0) {
        reverse_mem_copy(substr->chars, str->chars + start, len);
    }

    return substr;
}

char *string_to_c_string(const String *str) {
    if (str == NULL) {
        errno = EFAULT;
        return NULL;
    }

    char *cstr = malloc(str->len + 1);

    if (cstr == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    mem_copy(cstr, str->chars, str->len);
    cstr[str->len] = '\0';

    return cstr;
}

int string_equal(const String *str1, const String *str2) {
    /* NULL is not considered a string, so can't be equal to a string or even another NULL */
    if (str1 == NULL || str2 == NULL) {
        errno = EFAULT;  /* while NULL has a defined behavior here, it is still an error */
        return 0;
    }

    if (str1->len != str2->len) {
        return 0;
    }

    size_t i;
    for (i = 0; i < str1->len; i++) {
        if (str1->chars[i] != str2->chars[i]) {
            return 0;
        }
    }

    return 1;
}

int string_compare(const String *str1, const String *str2) {
    /* undefined behavior (other than setting errno) */
    if (str1 == NULL || str2 == NULL) {
        errno = EFAULT;
        return -1;  /* arbitrary */
    }

    size_t shortest_len = str1->len < str2->len ? str1->len : str2->len;

    size_t i;
    for (i = 0; i < shortest_len; i++) {
        int diff = str1->chars[i] - str2->chars[i];
        if (diff != 0) {
            return diff;
        }
    }

    /* a longer string is "bigger" e.g. abc - abcd < 0 */
    if (shortest_len == str1->len) {
        return -1;
    } else {
        return 1;
    }

    return 0;
}

void string_print(const String *str) {
    if (str == NULL) {
        errno = EFAULT;
        return;
    }

    size_t i;
    for (i = 0; i < str->len; i++) {
        printf("%c", str->chars[i]);
    }
}

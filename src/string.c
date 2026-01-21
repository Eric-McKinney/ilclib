#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ilc/string.h>


/* don't want to add string.h as dependency bc I'm stubborn */
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
        free(str);
        errno = ENOMEM;
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
        free(reverse);
        errno = ENOMEM;
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

String *substring(const String *str, long start, long end) {
    if (str == NULL) {
        errno = EFAULT;
        return NULL;
    }

    if ((unsigned long) labs(start) > str->len || (unsigned long) labs(end) > str->len) {
        errno = EDOM;
        return NULL;
    }

    if (start < 0) {
        start = str->len + start;
    }
    if (end < 0) {
        end = str->len + end + 1;  /* + 1 bc end is inclusive when negative, so translate to exclusive */
    }

    if (start > end) {
        errno = EINVAL;
        return NULL;
    }

    size_t len = (size_t) (end - start);

    String *substr = malloc(sizeof(String));

    if (substr == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    substr->chars = malloc(len * sizeof(char));

    if (substr->chars == NULL) {
        free(substr);
        errno = ENOMEM;
        return NULL;
    }

    substr->len = len;
    mem_copy(substr->chars, str->chars + start, len);

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

    if (str1->len == str2->len) {
        return 0;
    }

    /* a longer string is "bigger" e.g. abc - abcd < 0 */
    if (shortest_len == str1->len) {
        return -1;
    } else {
        return 1;
    }
}

void string_print(const String *str) {
    if (str == NULL) {
        printf("(null)");
        return;
    }

    size_t i;
    for (i = 0; i < str->len; i++) {
        printf("%c", str->chars[i]);
    }
}

void string_debug_print(const String *str) {
    if (str == NULL) {
        printf("(null)");
        return;
    }

    printf("{chars: \"");
    string_print(str);
    printf("\", len: %lu}", str->len);
}

/* like string_equal, but only compares up to str2 length from given start in str1 */
/* keeping this static bc it is made specially for string_contains & is easy to misuse */
static int bounded_string_equal(const String *str1, const String *str2, size_t start) {
    /* assuming: str2->len <= str1->len and that start + str2->len <= str1->len */
    size_t i;
    for (i = 0; i < str2->len; i++) {
        if (str1->chars[i + start] != str2->chars[i]) {
            return 0;
        }
    }

    return 1;
}

int string_contains(const String *str, const String *substr) {
    return string_contains_at(str, substr, NULL);
}

int string_contains_at(const String *str, const String *substr, size_t *idx) {
    if (str == NULL || substr == NULL) {
        errno = EFAULT;
        return 0;
    }

    if (str->len < substr->len) {
        return 0;
    }

    if (substr->len == 0) {
        if (idx != NULL) {
            *idx = 0;
        }
        return 1;
    }

    size_t i;
    for (i = 0; i <= str->len - substr->len; i++) {
        if (bounded_string_equal(str, substr, i)) {
            if (idx != NULL) {
                *idx = i;
            }
            return 1;
        }
    }

    return 0;
}

String *string_concat(const String *first, const String *second) {
    if (first == NULL || second == NULL) {
        errno = EFAULT;
        return NULL;
    }

    String *concat = malloc(sizeof(String));

    size_t concat_len = first->len + second->len;
    char *chars = malloc(concat_len);

    if (concat == NULL || chars == NULL) {
        free(concat);
        free(chars);

        errno = ENOMEM;
        return NULL;
    }

    mem_copy(chars, first->chars, first->len);
    mem_copy(chars + first->len, second->chars, second->len);

    concat->chars = chars;
    concat->len = concat_len;

    return concat;
}

int string_append(String *str, const String *to_append) {
    if (str == NULL || to_append == NULL) {
        errno = EFAULT;
        return EFAULT;
    }

    /* not strictly necessary but saves time */
    if (to_append->len == 0) {
        return 0;
    }

    size_t concat_len = str->len + to_append->len;
    char *chars = realloc(str->chars, concat_len);
    if (chars == NULL) {
        /* errno set by realloc when it fails */
        return ENOMEM;
    }

    mem_copy(chars + str->len, to_append->chars, to_append->len);

    str->chars = chars;
    str->len = concat_len;

    return 0;
}

static size_t *find_delims(const String *str, const String *delim, size_t *num_delims) {
    size_t *delim_locations = NULL;

    /* special case: delim is empty string, want to split on each char */
    if (delim->len == 0) {
        if (str->len == 0) {
            *num_delims = 0;
            return malloc(0);
        }

        *num_delims = str->len - 1;
        delim_locations = malloc(*num_delims * sizeof(size_t));
        if (delim_locations == NULL) {
            return NULL;
        }

        size_t i;
        for (i = 0; i < *num_delims; i++) {
            delim_locations[i] = i + 1;
        }

        return delim_locations;
    }

    size_t max_num_delims = str->len / delim->len;
    delim_locations = malloc(max_num_delims * sizeof(size_t));
    if (delim_locations == NULL) {
        return NULL;
    }

    *num_delims = 0;

    size_t i;
    for (i = 0; i <= str->len - delim->len; i++) {
        if (bounded_string_equal(str, delim, i)) {
            delim_locations[*num_delims] = i;
            *num_delims += 1;

            /* skip over rest of delim */
            i += delim->len - 1;  /* -1 to account for i++ at end of iteration */
        }
    }

    return delim_locations;
}

static String *create_string_splits(const String *str, size_t num_strs,
                                    size_t delim_len, size_t *delim_locations,
                                    size_t num_delims) {
    String *strs = malloc(num_strs * sizeof(String));
    if (strs == NULL) {
        return NULL;
    }

    size_t s_start = 0;
    size_t i;
    for (i = 0; i < num_delims; i++) {
        String *s = &strs[i];
        s->chars = str->chars + s_start;
        s->len = delim_locations[i] - s_start;

        s_start = delim_locations[i] + delim_len;
    }

    String *last_s = &strs[num_strs - 1];
    last_s->chars = str->chars + s_start;
    last_s->len = str->len - s_start;

    return strs;
}

int string_list_equal(const StringList *list1, const StringList *list2) {
    if (list1 == NULL || list2 == NULL) {
        errno = EFAULT;  /* same logic as in string_equal */
        return 0;
    }

    if (list1->len != list2->len) {
        return 0;
    }

    size_t i;
    for (i = 0; i < list1->len; i++) {
        if (!string_equal(&list1->strs[i], &list2->strs[i])) {
            return 0;
        }
    }

    return 1;
}

void string_list_print(const StringList *list) {
    if (list == NULL) {
        printf("(null)");
        return;
    }

    printf("[");

    size_t i;
    for (i = 0; i < list->len; i++) {
        if (i != 0) {
            printf(", ");
        }

        string_print(&list->strs[i]);
    }

    printf("]");
}

void string_list_debug_print(const StringList *list) {
    if (list == NULL) {
        printf("(null)");
        return;
    }

    printf("{len: %lu, strs: [", list->len);

    size_t i;
    for (i = 0; i < list->len; i++) {
        if (i != 0) {
            printf(", ");
        }
        string_debug_print(&list->strs[i]);
    }

    printf("]}");
}

StringList *string_split(const String *str, const String *delim) {
    if (str == NULL || delim == NULL) {
        errno = EFAULT;
        return NULL;
    }

    /* cannot split a string by a delimeter bigger than the string */
    if (delim->len > str->len) {
        errno = EINVAL;
        return NULL;
    }

    size_t num_delims;
    size_t *delim_locations = find_delims(str, delim, &num_delims);
    if (delim_locations == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    StringList *list = malloc(sizeof(StringList));
    if (list == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    /* always 1 more string than total instances of delimeter */
    list->len = num_delims + 1;

    String *strs = create_string_splits(str, list->len, delim->len,
                                        delim_locations, num_delims);
    if (strs == NULL) {
        free(delim_locations);
        errno = ENOMEM;
        return NULL;
    }

    list->strs = strs;

    free(delim_locations);
    return list;
}

String *string_join(const String *delim, const StringList *list) {
    if (delim == NULL || list == NULL) {
        errno = EFAULT;
        return NULL;
    }

    String *joined = malloc(sizeof(String));
    if (joined == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    joined->chars = NULL;  /* NULL works with string_append (realloc) */
    joined->len = 0;

    size_t i;
    for (i = 0; i < list->len; i++) {
        const String *s = &list->strs[i];
        int append_result;

        if (i != 0) {
            append_result = string_append(joined, delim);
            if (append_result != 0) {  /* errno already set by append */
                free_string(joined);
                return NULL;
            }
        }

        append_result = string_append(joined, s);
        if (append_result != 0) {  /* errno already set by append */
            free_string(joined);
            return NULL;
        }
    }

    return joined;
}

static size_t find_start_after_trim(const String *str, const StringList *to_trim) {
    size_t start_after_trim = 0;

    size_t i = 0, misses = 0;
    while (misses < to_trim->len) {
        String *s = &to_trim->strs[i];

        if (start_after_trim + s->len > str->len) {
            misses++;
            i = (i + 1) % to_trim->len;
            continue;
        }

        size_t j;
        for (j = start_after_trim; j <= str->len - s->len; j++) {
            if (bounded_string_equal(str, s, j)) {
                misses = 0;
                start_after_trim += s->len;
                j += s->len - 1;  /* -1 to account for j++ from for loop */
            } else {
                break;
            }
        }

        misses++;
        i = (i + 1) % to_trim->len;
    }

    return start_after_trim;
}

static size_t find_end_after_trim(const String *str, const StringList *to_trim) {
    size_t end_after_trim = str->len;

    size_t i = 0, misses = 0;
    while (misses < to_trim->len) {
        String *s = &to_trim->strs[i];

        if (end_after_trim - s->len < 0) {
            misses++;
            i = (i + 1) % to_trim->len;
            continue;
        }

        size_t j;
        for (j = end_after_trim - s->len; j >= 0; j--) {
            if (bounded_string_equal(str, s, j)) {
                misses = 0;
                end_after_trim -= s->len;
                j -= s->len - 1;  /* +1 to account for j-- from for loop */
            } else {
                break;
            }
        }

        misses++;
        i = (i + 1) % to_trim->len;
    }

    return end_after_trim;
}

String *string_ltrim(const String *str, const StringList *to_trim) {
    if (str == NULL || to_trim == NULL) {
        errno = EFAULT;
        return NULL;
    }

    size_t start = find_start_after_trim(str, to_trim);
    return create_string(str->chars + start, str->len - start);
}

String *string_rtrim(const String *str, const StringList *to_trim) {
    if (str == NULL || to_trim == NULL) {
        errno = EFAULT;
        return NULL;
    }

    size_t end = find_end_after_trim(str, to_trim);
    return create_string(str->chars, end);
}

String *string_trim(const String *str, const StringList *to_trim) {
    if (str == NULL || to_trim == NULL) {
        errno = EFAULT;
        return NULL;
    }

    size_t start = find_start_after_trim(str, to_trim);
    size_t end = find_end_after_trim(str, to_trim);

    if (end <= start) {
        return create_string("", 0);
    }

    return substring(str, start, end);
}

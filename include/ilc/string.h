#ifndef STRING_H
#define STRING_H

#include <stddef.h>

typedef struct {
    char *chars;  /* NOT a c string (no null terminator) */
    size_t len;
} String;

typedef struct {
    String *strs;
    size_t len;
} StringList;

String *create_string(const char *chars, size_t len);
String *string_copy(const String *str);
String *string_reverse(const String *str);
void free_string(String *str);
String *substring(const String *str, long start, long end);
char *string_to_c_string(const String *str);
int string_equal(const String *str1, const String *str2);
int string_compare(const String *str1, const String *str2);
void string_print(const String *str);
void string_debug_print(const String *str);
int string_contains(const String *str, const String *substr);
int string_contains_at(const String *str, const String *substr, size_t *idx);
String *string_concat(const String *first, const String *second);
int string_append(String *str, const String *to_append);
StringList *string_split(const String *str, const String *delim);

// TODO: String *string_join(const String *delim, const StringList *strs);

#endif

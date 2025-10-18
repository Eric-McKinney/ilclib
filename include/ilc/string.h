#ifndef STRING_H
#define STRING_H

#include <stddef.h>

typedef struct {
    char *chars;  /* NOT a c string (no null terminator) */
    size_t len;
} String;

String *create_string(const char *chars, size_t len);
String *string_copy(const String *str);
String *string_reverse(const String *str);
void free_string(String *str);
String *substring(const String *str, int start, int end);

#endif

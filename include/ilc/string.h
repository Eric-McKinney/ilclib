#ifndef STRING_H
#define STRING_H

#include <stddef.h>

typedef struct {
    char *chars;
    size_t len;
} String;

String *create_string(const char *chars, size_t len);
String *copy_string(const String *str);
void free_string(String *str);
size_t string_length(const String *str);
String *substring(const String *str, int start, int end);

#endif

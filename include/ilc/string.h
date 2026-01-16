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


/*
 * Allocates a String struct and initializes it. The chars field will be
 * initialized with a pointer to a dynamically allocated copy of exactly len
 * bytes from the given chars buffer.
 *
 * Errors (errno values):
 *   EFAULT: the chars argument was NULL
 *   ENOMEM: failed to allocate space (out of memory)
 *
 * Returns: a pointer to the string on success and NULL on failure.
 */
String *create_string(const char *chars, size_t len);


/*
 * Creates a copy of the given string. The contents of the chars buffer is
 * copied (i.e. a new buffer is allocated).
 *
 * Errors (errno values):
 *   EFAULT: the str argument was NULL
 *   ENOMEM: failed to allocate space (out of memory)
 *
 * Returns: a pointer to the new copy on success and NULL on failure.
 */
String *string_copy(const String *str);


/*
 * Creates a new string which has the given string's chars in reverse order.
 *
 * Errors (errno values):
 *   EFAULT: the str argument was NULL
 *   ENOMEM: failed to allocate space (out of memory)
 *
 * Returns: a pointer to the reversed string on success and NULL on failure.
 */
String *string_reverse(const String *str);


/*
 * Frees the memory allocated for a given string. Similar to free(), calling on
 * a NULL pointer is permissible and will do nothing. Strings should only be
 * freed once and should not be used after being freed.
 *
 * This function does not have any associated errors and does not return
 * anything.
 */
void free_string(String *str);


/*
 * Create a new string which has chars copied from the given string within the
 * range of the given start and end indices. The start index is inclusive
 * while the end index is exclusive. Additionally, the start and end indices
 * may be negative to denote the index given by adding the string's length to
 * it. For example, an index of -1 denotes the last character of a string as
 * long as the string has a length of 1 or more.
 *
 * In general the bounds for the start and end indices are:
 * -len <= start <= len
 * -len <= end <= len
 * where len is the length of the given string.
 *
 * The start index must refer to a character that is the same as or occurs
 * before the character referred to by the end index. If both indices are
 * positive then this simply means start <= end. If the start and end indices
 * are equal, then the resulting string will be empty (i.e. its length is 0).
 *
 * The following are valid ranges for a string of length 3:
 * 0 to 3, 1 to 1, 2 to -1, -2 to -1
 *
 * Errors (errno values):
 *   EFAULT: the str argument was NULL
 *   EDOM: either start or end (or both) were out of bounds (shown above)
 *   EINVAL: the end of the range occurs before the start of the range
 *   ENOMEM: failed to allocate space (no memory)
 *
 * Returns: a pointer to the substring on success, NULL on failure
 */
String *substring(const String *str, long start, long end);


/*
 * Get a null character terminated buffer of the chars in the given string. The
 * buffer should be freed later using free() from the stdlib.
 *
 * Errors (errno values):
 *   EFAULT: the str argument was NULL
 *   ENOMEM: failed to allocate space (no memory)
 *
 * Returns: a pointer to the c-style string on success, NULL on failure
 */
char *string_to_c_string(const String *str);


/*
 * Check if two strings are equal. Strings are equal if they have the same
 * length and characters. A NULL pointer is not considered a string, so any
 * call to this function with a NULL pointer is effectively undefined behavior.
 * That being said, this function will return 0 in such cases, even if both
 * arguments are NULL.
 *
 * Errors (errno values):
 *   EFAULT: either str1, str2, or both were NULL
 *
 * Returns: 1 if the strings are equal, 0 if they aren't equal
 */
int string_equal(const String *str1, const String *str2);


/*
 * Compare two strings by their characters. Useful for sorting strings
 * alphebetically (a -> z is smallest -> largest). A longer string is
 * considered greater if the characters up to that point are equal (e.g. a < aa).
 * A NULL pointer is not considered a string, so any call to this function with
 * a NULL pointer is effectively undefined behavior. That being said, this
 * function will return -1 in such cases, even if both arguments are NULL.
 *
 * Errors (errno values):
 *   EFAULT: either str1, str2, or both were NULL
 *
 * Returns: 0 (strings are equal), > 0 (str1 greater), < 0 (str2 greater)
 */
int string_compare(const String *str1, const String *str2);


/*
 * Print the characters of the string to stdout.
 *
 * Errors (errno values):
 *   EFAULT: the str argument was NULL
 *
 * Returns: nothing.
 */
void string_print(const String *str);


/*
 * Print the string's characters and length in the following format:
 * {chars: "str characters", len: 14}
 *
 * If the str argument is NULL, then NULL will be printed.
 *
 * No errors possible.
 *
 * Returns: nothing.
 */
void string_debug_print(const String *str);


/*
 * Check if the given string (str) contains another string (substr).
 *
 * For a string to be contained by another string it must be a substring of
 * that string. The arguments can be any two strings. The parameter names str
 * and substr are only to make it clear which string is being checked for being
 * contained.
 *
 * Every string contains the empty string because substring(str, 0, 0) will
 * return the empty string for any string. Note that a NULL pointer is not
 * considered a string.
 *
 * Errors (errno values):
 *   EFAULT: str, substr, or both were NULL
 *
 * Returns: 1 if str contains substr, 0 if it doesn't.
 */
int string_contains(const String *str, const String *substr);


/*
 * Check if the given string (str) contains another string (substr) and write
 * the index where the first instance of substr is found within str to the
 * given size_t pointer (idx). If the given size_t pointer (idx) is NULL, then
 * it will be ignored.
 *
 * For a string to be contained by another string it must be a substring of
 * that string. The arguments can be any two strings. The parameter names str
 * and substr are only to make it clear which string is being checked for being
 * contained.
 *
 * Every string contains the empty string at index 0 because
 * substring(str, 0, 0) will return the empty string for any string. Note that
 * a NULL pointer is not considered a string.
 *
 * Errors (errno values):
 *   EFAULT: str, substr, or both were NULL
 *
 * Returns: 1 if str contains substr, 0 if it doesn't.
 */
int string_contains_at(const String *str, const String *substr, size_t *idx);


/*
 * Create a new string which has the characters of the first string followed by
 * the characters of the second string.
 *
 * Errors (errno values):
 *   EFAULT: first, second, or both were NULL
 *   ENOMEM: failed to allocate space (no memory)
 *
 * Returns: a pointer to the new string on success, NULL on failure.
 */
String *string_concat(const String *first, const String *second);


/*
 * Append the characters of the string to_append to the string str.
 *
 * Errors (errno values):
 *   EFAULT: str, to_append, or both were NULL
 *   ENOMEM: failed to allocate space (no memory)
 *
 * Returns: 0 on success, errno of error on failure (errno is set too).
 */
int string_append(String *str, const String *to_append);


/*
 * Check if two string lists are equal. String lists are considered equal if
 * their lengths are the same and each list has the same sequence of strings.
 *
 * Errors (errno values):
 *   EFAULT: list1, list2, or both were NULL
 *
 * Returns: 1 if the lists are equal, 0 if they are not equal.
 */
int string_list_equal(const StringList *list1, const StringList *list2);


/*
 * Print the list's strings and length in the following format:
 * {len: 2, strs: [{chars: "example", len: 7}, {chars: "list", len: 4}]}
 *
 * If the list is NULL, then NULL will be printed.
 *
 * No errors are possible.
 *
 * Returns: nothing.
 */
void string_list_debug_print(const StringList *list);


/*
 * Split a string into a string list by a given delimeter. The list will
 * consist of the substrings from beginning to end of the string around all
 * instances of the delimeter. The chars of each string in the list are not
 * distinct buffers, instead pointing to locations in the original string's
 * chars buffer. This means that the string list created by this function can
 * be freed by one call to free(). It also means that the original string
 * should not be freed before the string list is freed.
 *
 * If the delimeter occurs at the beginning, end, or immediately after another
 * instance of the delimeter, the list will contain an empty string in that
 * location.
 *
 * ex: string_split(",,", ",") results in the list
 * ["", "", ""]
 *
 * Errors (errno values):
 *   EFAULT: str, delim, or both were NULL
 *   EINVAL: the delimeter is longer than the string to split
 *   ENOMEM: failed to allocate space (no memory)
 *
 * Returns: a pointer to the string list on success, NULL on failure.
 */
StringList *string_split(const String *str, const String *delim);
String *string_join(const String *delim, const StringList *list);

// TODO: trim, ltrim, rtrim

#endif

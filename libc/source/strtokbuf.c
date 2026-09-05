// strntok bring back tokens from the string, kernel use only, this is a limited version

#include <stdio.h>

#include <string.h>

// is the character a delimiter

bool isdelim(char c, const char* const restrict delims) {
    if (!delims) return false;

    for (size_t i = 0; i < strlen(delims); i++) {
        if (c == delims[i]) return true;
    }
    // not in our list
    return false;
}

// Get a series of substrings
// as long as the string remains the same gets the next token
// Parameters:  str -- The string to scan, if null use last string
//              delim -- a list of characters that are delimiterssssssss

// Returns:     a pointer to the buffer, or null ptr if end of string or an error occurs

char* strtokbuf(const char * const s, const char * const delims, char* buff, int buflen, int start, int *newstart) {
    // validate parameters
    if (!delims) return nullptr;
    if (!s) return nullptr;
    if (!buff) return nullptr;
    if (buflen <= 0) return nullptr;
    if (start < 0) return nullptr;
    if (start > buflen) return nullptr;
    if (!newstart) return nullptr;

    // all is OK
    // remove leading delimiters
    while ((start < buflen) && (s[start] != '\0') && (isdelim(s[start],delims))) {
        start++;
    }

    // check if we are at end of string
    // if so string is empty, no token
    if (s[start] == '\0') return nullptr;

    // set newstart so we can increment
    *newstart = start;

    // copy token to buffer
    int count = 0;
 
    // copy while we don't overrun the buffer, it isn't end of string, and it isn't a delimiter
    while ((count < (buflen - 1)) &&
           (s[start + count] != '\0') &&
           (!isdelim(s[start + count], delims))) {
        buff[count] = s[start + count];
        count++;
        (*newstart)++;
    }

    // trailing null
    buff[count] = '\0';

    // if we stopped on a delimiter go past it
    if ((s[*newstart] != '\0') && 
        (isdelim(s[*newstart],delims))) {
        (*newstart)++;
    }

    // token is moved

    return buff;
}
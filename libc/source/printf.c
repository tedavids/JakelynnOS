// our printf command

#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <tty.h>
#include <stdio.h>
#include <string.h>

bool print(const char* string) 
{
    return ttyWriteString(string);
} // static bool print(const char* data, size_t length)

int printf(const char* restrict format, ...) 
{
    va_list parameters;
    va_start(parameters, format);

    char buffer[256] = "";

    snprintfint(buffer, sizeof(buffer), format, &parameters);
    size_t written =  strlen(buffer);
    if (!ttyWriteString(buffer)) return 0;


    return (int) written;

} // int printf(const char* restrict format, ...)
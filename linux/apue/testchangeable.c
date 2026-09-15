
#include "apue.h"
#include "stdarg.h"

void func(int a, ...)
{
    int *temp = &a;
    temp++;
    for (int i = 0; i < a; ++i)
    {
        printf("%d,", *temp);
        temp++;
    }
    printf("\n");
}

void X_DEBUG(const char *fmt,...)
{
    char str[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(str, 1024, fmt, args);
    va_end(args);
}

int main(int argc, char const* argv[])
{
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    X_DEBUG("this is abc %s\n", "abc");
    err_msg("heheda");
    return 0;
}

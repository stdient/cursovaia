#include "../include/String.h"

#include <iconv.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int convertStrToInt(char *str)
{
    size_t str_size = mstrlen(str);
    int integer = 0;

    for (size_t i = 0; i < str_size; ++i)
    {
        integer += (str[i] - '0') * pow(10, str_size - 1 - i);
    }

    return integer;
}

char *convertStr(char *str)
{
    size_t str_size = mstrlen(str) + 1;
    char *outbuf = (char *)malloc(str_size * 2);
    iconv_t cd = iconv_open("UTF-8", "CP866");
    if (cd == (iconv_t)-1)
    {
        fprintf(stderr, "Cannot convert!\n");
        exit(EXIT_FAILURE);
    }

    char inbuf[str_size];
    for (int i = 0; i < (int)str_size; ++i)
    {
        inbuf[i] = str[i];
    }
    size_t inleft = str_size;
    size_t outleft = str_size * 2;
    char *outptr = (char *)outbuf;
    char *inptr = (char *)&inbuf;

    int rc = iconv(cd, &inptr, &inleft, &outptr, &outleft);
    if (rc == -1)
    {
        fprintf(stderr, "Cannot convert!\n");
        exit(EXIT_FAILURE);
    }
    iconv_close(cd);

    return outbuf;
}

size_t mstrlen(const char *str)
{
    size_t str_size = 0;
    while (str[str_size] != '\0')
    {
        str_size++;
    }
    return str_size;
}

char *mstrcat(char *dest, char *src)
{
    size_t dest_size = mstrlen(dest);
    size_t src_size = mstrlen(src);

    size_t res_size = dest_size + src_size + 1;
    char *res = (char *)malloc(res_size);

    for (size_t i = 0; i < res_size; ++i)
    {
        if (i < dest_size)
            res[i] = dest[i];
        else
            res[i] = src[i];
    }

    return res;
}

void mstrcpy(char *toHere, char *fromHere)
{
    for (size_t i = 0; i < mstrlen(fromHere) + 1; ++i)
    {
        toHere[i] = fromHere[i];
    }
}

char *mstrtok(char *s, const char *delim)
{
    char *token;
    static char *prev_token;
    static char *next_token;
    int rem = 0;
    int start = 0;

    if (s)
    {
        token = s;
        prev_token = 0;
    }
    else
    {
        token = next_token;
    }

    for (int i = 0; token[i] != '\0'; ++i)
    {
        if (start)
            rem++;

        for (int j = 0; delim[j] != '\0' && !start; ++j)
        {
            if (token[i] == delim[j])
            {
                token[i] = '\0';
                next_token = token + i;
                start = 1;
            }
        }
    }

    if (prev_token == token)
        token = NULL;

    if (rem)
        next_token++;

    prev_token = token;

    return token;
}
#pragma once
#include <stddef.h>

char *convertStr(char *str);
size_t mstrlen(const char *str);
char *mstrcat(char *dest, char *src);
void mstrcpy(char *toHere, char *fromHere);
// TODO несколько разграничителей подряд
char *mstrtok(char *s, const char *delim);
int convertStrToInt(char *str);
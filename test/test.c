#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *mstrtok(char *s, const char *delim) {
    char *token;
    static char *prev_token;
    static char *next_token;
    int rem = 0;
    int start = 0;

    if (s) {
        token = s;
        prev_token = 0;
    } else {
        token = next_token;
    }

    for (int i = 0; token[i] != '\0'; ++i) {
        if (start) rem++;

        for (int j = 0; delim[j] != '\0' && !start; ++j) {
            if (token[i] == delim[j]) {
                token[i] = '\0';
                next_token = token + i;
                start = 1;
            }
        }
    }

    if (prev_token == token) token = NULL;

    if (rem) next_token++;

    prev_token = token;

    return token;
}

int main() {
    system("clear");
    char str[] = "52-91-56";

    char *token = mstrtok(str, "-");
    while (token) {
        printf("token = %s\n\n", token);
        token = mstrtok(NULL, "-");
    }

    return 0;
}
#include <stdio.h>

#define C2H(C) ({                                               \
            int _C = (C);                                       \
            '0' <= _C && _C <= '9' ? _C - '0' : 0xa + _C - 'a'; \
        })

static int interpret_sequence(void) {
    /* See https://wiki.xaseco.org/wiki/Text_formatting */

    int c = getchar();
    switch (c) {
    /* Color */
    case '0' ... '9':
    case 'a' ... 'f':
        {
            /* TODO: check if they are garented to evaluate in this order */
            int r = C2H(c), g = C2H(getchar()), b = C2H(getchar());
            printf("\e[38;2;%d;%d;%dm", r * 16, g * 16, b * 16);
        }
        break;

    /* Default color */
    case 'g':
        printf("\e[39;49m");
        break;

    /* Italic */
    case 'i':
        printf("\e[3m");
        break;

    /* Shadowed */
    case 's':
        /* Bold */
        printf("\e[1m");
        break;

    /* Reset all */
    case 'z':
        printf("\e[m");
        break;

    case '$':
        putchar('$');
        break;
    }
}

int main(void) {
    /* FIXME: It may be really slow to read and write character by
     * character, need to benchmark.
     */
    for (;;) {
        int c = getchar();
        switch (c) {
        case EOF:
            goto out;
        case '$':
            interpret_sequence();
            break;

        /* TODO: make this a command line argument */
        case '\n':
            printf("\e[m\n");
            break;
        default:
            putchar(c);
        }
    }    
out:

    /* TODO: better error reporting */
    return ferror(stdin);
}

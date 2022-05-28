#include <stdio.h>

#define C2H(C) ({                                               \
            int _C = (C);                                       \
            '0' <= _C && _C <= '9' ? _C - '0' : 0xa + _C - 'a'; \
        })

static int interpret_sequence(void) {
    int c = getchar();
    switch (c) {
    case '0' ... '9':
    case 'a' ... 'f':
        {
            /* TODO: check if they are garented to evaluate in this order */
            int r = C2H(c), g = C2H(getchar()), b = C2H(getchar());
            printf("\e[38;2;%d;%d;%dm", r * 16, g * 16, b * 16);
        }
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
            puts("\e[0m");
            break;
        default:
            putchar(c);
        }
    }    
out:

    /* TODO: better error reporting */
    return ferror(stdin);
}

#include <stdio.h>

static int hex_char(int c) {
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('A' <= c && c <= 'F')
        return 0xa + c - 'A';
    else
        return 0xa + c - 'a';
}

static int interpret_sequence(void) {
    /* See https://wiki.xaseco.org/wiki/Text_formatting */

    int c = getchar();
    switch (c) {
    /* Color */
    case '0' ... '9':
    case 'A' ... 'F':
    case 'a' ... 'f':
        {
            /* TODO: check if they are garented to evaluate in this order */
            int r = hex_char(c), g = hex_char(getchar()), b = hex_char(getchar());
            printf("\e[38;2;%d;%d;%dm", r * 0xf, g * 0xf, b * 0xf);
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
    case 'o':
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

#include <argp.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = "<lucas@ransan.tk>";

static char doc[] = PACKAGE_NAME " -- TrackMania/ManiaPlanet text formatting to terminal sequences";
static char args_doc[] = "[FILE]";

static struct argp_option options[] = {
    { "nl-reset", 'n', 0, 0, "reset style at each new line" },
    { 0 }
};

struct args {
    char *file;
    bool nl_reset;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct args *args = state->input;
    switch (key) {
    case 'n':
        args->nl_reset = true;
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num >= 1)
            argp_usage(state);
        args->file = arg;
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

static int hex_char(int c) {
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('A' <= c && c <= 'F')
        return 0xa + c - 'A';
    else
        return 0xa + c - 'a';
}

static int interpret_sequence(FILE *in_file) {
    /* See https://wiki.xaseco.org/wiki/Text_formatting */

    int c = getc(in_file);
    switch (c) {
    /* Color */
    case '0' ... '9':
    case 'A' ... 'F':
    case 'a' ... 'f':
        {
            /* TODO: check if they are garented to evaluate in this order */
            int r = hex_char(c), g = hex_char(getc(in_file)), b = hex_char(getc(in_file));
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

int main(int argc, char **argv) {
    struct args args = {
        .file = "-",
        .nl_reset = false,
    };

    argp_parse(&argp, argc, argv, 0, 0, &args);

    FILE *in_file;
    bool is_stdin = strcmp(args.file, "-") == 0;
    if (is_stdin) {
        in_file = stdin;
    } else {
        in_file = fopen(args.file, "r");
        if (!in_file)
            err(1, "%s", args.file);
    }

    /* FIXME: It may be really slow to read and write character by
       character, need to benchmark. */
    for (;;) {
        int c = getc(in_file);
        switch (c) {
        case EOF:
            goto out;
        case '$':
            interpret_sequence(in_file);
            break;

        case '\n':
            if (args.nl_reset)
                printf("\e[m\n");
            else
                putchar('\n');
            break;
        default:
            putchar(c);
        }
    }    
out:

    int res = ferror(in_file);
    if (!is_stdin)
        fclose(in_file);

    return res;
}

#include "cachelab.h"
#include "ctype.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEBUG
// #define DEBUG_PARSE_ARGS
#define DEBUG_FILE_READ

#ifdef DEBUG
#define ASSERT(COND) assert(COND)
#define REQUIRES(COND) assert(COND)
#define ENSURES(COND) assert(COND)

#else
#define ASSERT(COND) ((void)0)
#define REQUIRES(COND) ((void)0)
#define ENSURES(COND) ((void)0)
#define IF_DEBUG(CMD) ((void)0)

#endif

void print_usage(void) {
    printf("Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <trace>\n");
    printf("  -h\t\tPrint this help message.\n");
    printf("  -v\t\tVerbose mode: Reports effects of each memory operation\n");
    printf("  -s <s>\tNumber of set index bits. (there are 2**s sets)\n");
    printf("  -E <E>\tNumber of lines per set (associativity)\n");
    printf("  -b <b>\tNumber of block bits. (there are 2**b blocks)\n");
    printf("  -t <trace>\tFile name of the memory trace to process\n\n");
}

void validate(const int *m, int *s, int *b, int *E, char *tFile, int *t) {

    if (*s == -1 || *b == -1 || *E == -1 || tFile == NULL) {
        fprintf(stderr, "Incorrect/Missing arguments.\n");
        print_usage();
        exit(-1);
    } else if (*s < 0 || *s > *m) {
        fprintf(stderr, "Error: Invalid value. s not in [0, %d]\n", *m);
        exit(-1);
    } else if (*b < 0 || *b > *m) {
        fprintf(stderr, "Error: Invalid value. b not in [0, %d]\n", *m);
        exit(-1);
    } else if (*E <= 0) {
        fprintf(stderr, "Error: Invalid value: E <= 0\n");
        exit(-1);
    } else if (*s + *b > *m) {
        fprintf(stderr, "Error: s + b > word_length (%d) | (s = %d, b = %d)\n",
                *m, *s, *b);
        exit(-1);
    } else {
        *t = *m - (*s + *b);
    }
}

void parse_args(int argc, char **argv, int *s, int *b, int *E, int *vFlag,
                char **tFile) {
    int c = 0;

    while ((c = getopt(argc, argv, "hvs:b:E:t:")) != -1) {
        switch (c) {
        case 'h':
            print_usage();
            exit(0);
            break;
        case 'v':
            *vFlag = 1;
            break;
        case 's':
            *s = (int)strtoul(optarg, NULL, 10);
            break;
        case 'b':
            *b = (int)strtoul(optarg, NULL, 10);
            break;
        case 'E':
            *E = (int)strtoul(optarg, NULL, 10);
            break;
        case 't':
            *tFile = optarg;
            break;
        default:
            fprintf(stderr, "Missing required arguments.");
            exit(-1);
        }
    }
}

int main(int argc, char **argv) {

    const int m = (int)sizeof(size_t) * 8; // Num bits in an addr
    int s = -1, b = -1, E = -1, vFlag = 0;
    int t = -1;
    char *tFile = NULL;

    parse_args(argc, argv, &s, &b, &E, &vFlag, &tFile);

#ifdef DEBUG_PARSE_ARGS
    printf("vFlag: %d\n", vFlag);
    printf("s: %d\n", s);
    printf("b: %d\n", b);
    printf("E: %d\n", E);
    printf("tFile: %s\n", tFile);
#endif

    validate(&m, &s, &b, &E, tFile, &t);

    FILE *traceFile;

    char op;
    unsigned long addr;
    int size;
    int ptr;

    traceFile = fopen(tFile, "rt");
    if (!traceFile) {
        fprintf(stderr, "Error opening '%s'\n", tFile);
        return -1;
    }

    while ((ptr = fscanf(traceFile, "%c %lx,%d\n", &op, &addr, &size)) != EOF) {
        if (!ptr) {
            fprintf(stderr, "Error: Invalid Read: %s\n", tFile);
            exit(1);
        }

#ifdef DEBUG_FILE_READ
        printf("%c %lx,%d\n", op, addr, size);
#endif
    }

    fclose(traceFile);

    return 0;
}

#include "cachelab.h"
#include "ctype.h"
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEBUG
// #define DEBUG_PARSE_ARGS
// #define DEBUG_FILE_READ
#define DEGUB_CACHE

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
        exit(1);
    } else if (*s < 0 || *s > *m) {
        fprintf(stderr, "Error: Invalid value. s not in [0, %d]\n", *m);
        exit(1);
    } else if (*b < 0 || *b > *m) {
        fprintf(stderr, "Error: Invalid value. b not in [0, %d]\n", *m);
        exit(1);
    } else if (*E <= 0) {
        fprintf(stderr, "Error: Invalid value: E <= 0\n");
        exit(1);
    } else if (*s + *b > *m) {
        fprintf(stderr, "Error: s + b > word_length (%d) | (s = %d, b = %d)\n",
                *m, *s, *b);
        exit(1);
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
            exit(1);
        }
    }
}

typedef struct line {
    long tag;
    int valid;
    struct line *next;
} Line;

typedef struct set {
    int E;
    size_t size;
    Line *head; // LRU
    Line *tail; // MRU
} Set;

typedef struct cache {
    unsigned long S;
    unsigned long hits;
    unsigned long misses;
    unsigned long evictions;
    unsigned long dirty_evictions;
    Set **Sets; // Array of Sets
} Cache;

Set *setNew(int E) {

    Set *S = malloc(sizeof(Set));
    if (S == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }
    S->head = NULL;
    S->tail = NULL;
    S->size = 0;
    S->E = E;

    return S;
}

void setFree(Set *S) {
    if (S == NULL)
        return;

    if (S->size == 0) {
        free(S);
        return;
    }

    Line *L = S->head;

    while (L != S->tail) {
        Line *curr = L;
        L = L->next;
        free(curr);
    }

    free(S->tail);
    free(S);
}

bool setInsertTail(Set *S, long tag) {
    if (S == NULL)
        return false;

    Line *new_line = malloc(sizeof(Line));

    if (new_line == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }

    new_line->tag = tag;

    if (S->size == 0) {
        S->head = new_line;
        S->tail = new_line;
    } else {
        S->tail->next = new_line;
        S->tail = S->tail->next;
    }
    S->size++;
    return true;
}

bool setRemoveHead(Set *S) {
    /* You need to fix up this code. */
    if (S == NULL || S->size == 0)
        return false;
    Line *evict_line = S->head;
    S->size--;
    S->head = (S->size == 0) ? NULL : S->head->next;
    free(evict_line);
    return true;
}

void setPrint(Set *S, unsigned long set_num) {

    if (S == NULL)
        return;

    printf("Set: %lu\n", set_num);

    Line *ptr = S->head;
    printf("  | LRU |\n");
    printf("  ************\n");

    for (int i = 0; i < S->E; i++) {
        if (i < (int)S->size) {
            ASSERT(ptr != NULL);
            printf("  Valid: %d\n", ptr->valid);
            printf("  tag: %lx\n", ptr->tag);
            if (ptr == S->tail)
                printf("\t\t| MRU |\n");
            ptr = ptr->next;
        } else {
            printf("  Valid: 0\n");
            printf("  tag:  --\n");
        }
        printf("  ************\n");
    }
}

Cache *cacheNew(int s, int E) {

    unsigned long S = 1UL << ((unsigned long)s);
    Cache *C = calloc(1, sizeof(Cache));
    if (C == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }

    C->S = S;

    C->Sets = malloc(S * sizeof(Set *));
    if (C->Sets == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }

    for (unsigned long i = 0; i < S; i++) {
        C->Sets[i] = setNew(E);
    }

    return C;
}

void cacheFree(Cache *C) {
    if (C == NULL)
        return;

    for (unsigned long i = 0; i < C->S; i++) {
        setFree(C->Sets[i]);
    }

    free(C->Sets);
    free(C);
}

void cachePrint(Cache *C) {

    if (C == NULL)
        return;

    printf("Puru's Cache Sim\n");
    printf(" S: %lu\n", C->S);
    printf(" E: %d\n", C->Sets[0]->E);
    printf(" hits: %lu\n", C->hits);
    printf(" misses: %lu\n", C->misses);
    printf(" evictions: %lu\n", C->evictions);
    printf(" dirty_evictions: %lu\n", C->dirty_evictions);
    printf("-------------------------------------------\n");

    for (unsigned long i = 0; i < C->S; i++) {
        setPrint(C->Sets[i], i);
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

    Cache *cache = cacheNew(s, E);

#ifdef DEGUB_CACHE
    cachePrint(cache);
#endif

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
    cacheFree(cache);

    return 0;
}

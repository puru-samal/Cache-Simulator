<<<<<<< HEAD
=======
/*

Cache Simulator.
        by Puru

This program simulates memory accesses via a cache and the number
of hits, misses, dirty bytes in cache and dirty bytes evicted
based on cache configuration and the operations provided.

 Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <trace>\n");
    -h          Prints help message
    -v          Verbose mode: Reports effects of each memory operation
    -s <s>      Number of set index bits. (there are 2**s sets)
    -E <E>      Number of lines per set (associativity)
    -b <b>      Number of block bits. (there are 2**b blocks)
    -t <trace>  File name of the memory trace to process

Format for trace files: op addr,size

The Cache is implemented as an array of singly-linked lists as it simplifies
the process of maintaining order of elements based on how recently they were
used, which is required to handle eviction under the LRU policy.
Here is a quick summary of how the program works:

- Arguments are parsed and a new Cache is created
- The file provided as an argument is parsed


*/

>>>>>>> 126ffe2 (trans_32 added)
#include "cachelab.h"
#include "ctype.h"
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

<<<<<<< HEAD
#define DEBUG
// #define DEBUG_PARSE_ARGS
// #define DEBUG_FILE_READ
#define DEGUB_CACHE

=======
/* Uncommenting this definition will enable
   the assertions used while writing the program */

// #define DEBUG
>>>>>>> 126ffe2 (trans_32 added)
#ifdef DEBUG
#define ASSERT(COND) assert(COND)
#define REQUIRES(COND) assert(COND)
#define ENSURES(COND) assert(COND)

#else
#define ASSERT(COND) ((void)0)
#define REQUIRES(COND) ((void)0)
#define ENSURES(COND) ((void)0)
#define IF_DEBUG(CMD) ((void)0)
<<<<<<< HEAD

#endif

=======
#endif

/* Uncommenting this line will print the prosessed arguments */

// #define DEBUG_PARSE_ARGS

/* Uncommenting this line will pretty print the traces
   along with some information regarding program state.
   This is followed by a pretty print of the simulated
   cache and all it's contents. */

// #define DEBUG_CACHE

/************************* Processing Commang Line Arguments
 * *************************************/

/* Helper function that prints usuage whrn the -h flag is set */

>>>>>>> 126ffe2 (trans_32 added)
void print_usage(void) {
    printf("Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <trace>\n");
    printf("  -h\t\tPrint this help message.\n");
    printf("  -v\t\tVerbose mode: Reports effects of each memory operation\n");
    printf("  -s <s>\tNumber of set index bits. (there are 2**s sets)\n");
    printf("  -E <E>\tNumber of lines per set (associativity)\n");
    printf("  -b <b>\tNumber of block bits. (there are 2**b blocks)\n");
    printf("  -t <trace>\tFile name of the memory trace to process\n\n");
}

<<<<<<< HEAD
=======
/* Helper function that checks if the arguments provided are valid
   and prints and error message if invavid before calling exit(1) */

>>>>>>> 126ffe2 (trans_32 added)
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

<<<<<<< HEAD
=======
/* Helper function that parses the command line arguments and appropriately
   sets the values of the addresses provided. This function does not check for
   the corectness og arguments. That is handled by vallidate. */

>>>>>>> 126ffe2 (trans_32 added)
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

<<<<<<< HEAD
typedef struct line {
    long tag;
=======
/************************* Data Structures for the Cache Simulator
 * *************************************/

// For convenience
typedef unsigned long word_t;

/* A member of Set containing state info regarding valid
    and dirty bits and points to the next line */

typedef struct line {
    word_t dirty;
    word_t tag;
>>>>>>> 126ffe2 (trans_32 added)
    int valid;
    struct line *next;
} Line;

<<<<<<< HEAD
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
=======
/* A member of Cache containing state info regarding
    number of lines and pointers to MRU line and LRU linen*/

typedef struct set {
    word_t E;
    size_t size;
    Line *LRU; // LRU
    Line *MRU; // MRU
} Set;

/* Struct representing the Cache as an array of linked lists
    which for our purposes are sets. Also contains state information
    requires for allocaing the appropriate amount of space and
    indexing into the correct set. */

typedef struct cache {
    word_t S;
    word_t E;
    word_t B;
    word_t s;
    word_t b;
    word_t t;
    csim_stats_t stats;
    Set **Sets;
} Cache;

/* Creates a new Set and returns a pointer to it. If space could not
   be allocated, calls exit(1) */

Set *setNew(word_t E) {
>>>>>>> 126ffe2 (trans_32 added)

    Set *S = malloc(sizeof(Set));
    if (S == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }
<<<<<<< HEAD
    S->head = NULL;
    S->tail = NULL;
    S->size = 0;
    S->E = E;

    return S;
}

=======
    S->LRU = NULL;
    S->MRU = NULL;
    S->size = 0;
    S->E = E;

    ENSURES(S != NULL);

    return S;
}

/* Frees a Set and all its members */

>>>>>>> 126ffe2 (trans_32 added)
void setFree(Set *S) {
    if (S == NULL)
        return;

    if (S->size == 0) {
        free(S);
        return;
    }

<<<<<<< HEAD
    Line *L = S->head;

    while (L != S->tail) {
=======
    Line *L = S->LRU;

    while (L != S->MRU) {
>>>>>>> 126ffe2 (trans_32 added)
        Line *curr = L;
        L = L->next;
        free(curr);
    }

<<<<<<< HEAD
    free(S->tail);
    free(S);
}

bool setInsertTail(Set *S, long tag) {
    if (S == NULL)
        return false;
=======
    free(S->MRU);
    free(S);
}

/* Evicts the LRU line and returns 1 of the line evicted was dirty.
   Returns 0 otherwise. Requires for cache stat calculation  */

int setRemoveLRU(Set *S) {

    REQUIRES(S != NULL && S->size >= 0);

    Line *evict_line = S->LRU;
    S->size--;
    S->LRU = (S->size == 0) ? NULL : S->LRU->next;

    // Used for checking if bytes are dirty during eviction
    int dirty = (int)evict_line->dirty; // Dirty Bit 0 or 1

    free(evict_line);
    return dirty;
}

/* Inserts an element so it has to be MRU. Returns 1 is insertion
    caused an eviction. Returns 0 otherwise. Return value is used to
    detect evictions. */

int setInsertMRU(Set *S, word_t tag, word_t *is_dirty) {

    REQUIRES(S != NULL);
    REQUIRES(is_dirty != NULL);

    // Takes in a pointer that indicates if byte is dirty
>>>>>>> 126ffe2 (trans_32 added)

    Line *new_line = malloc(sizeof(Line));

    if (new_line == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }

    new_line->tag = tag;
<<<<<<< HEAD

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
=======
    new_line->valid = 1;
    new_line->dirty = *is_dirty;

    if (S->size == 0) {
        S->LRU = new_line;
        S->MRU = new_line;
    } else {
        S->MRU->next = new_line;
        S->MRU = S->MRU->next;
    }
    S->size++;

    // Eviction
    if (S->size > (size_t)S->E) {
        *is_dirty = (word_t)setRemoveLRU(S);
        return 1; // indicates eviction
    }

    // No eviction
    return 0;
}

/* In the case of a hit, replaces the MRU with curr. */

void setReplaceMRU(Set *S, Line *curr, Line *prev) {

    REQUIRES(S != NULL || curr != NULL);
    REQUIRES(prev == NULL || prev->next == curr);

    if (curr == S->MRU)
        return;

    if (curr == S->LRU) {
        S->LRU = curr->next;
        S->MRU->next = curr;
        S->MRU = curr;
        curr->next = NULL;
    } else {
        prev->next = curr->next;
        S->MRU->next = curr;
        S->MRU = curr;
        curr->next = NULL;
    }

    ENSURES(S->MRU == curr);
}

/* A function called within cachePrint to
pretty print a representation of a cache */

void setPrint(Set *S, word_t set_num) {
>>>>>>> 126ffe2 (trans_32 added)

    if (S == NULL)
        return;

    printf("Set: %lu\n", set_num);

<<<<<<< HEAD
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
=======
    Line *ptr = S->LRU;
    printf("  | LRU |\n");
    printf("  ************\n");

    for (word_t i = 0; i < S->E; i++) {
        if (i < S->size) {
            ASSERT(ptr != NULL);
            printf("  Valid: %d\n", ptr->valid);
            printf("  tag: %lx\n", ptr->tag);
            printf("  dirty: %lx\n", ptr->dirty);
            if (ptr == S->MRU)
                printf(" | MRU |\n");
>>>>>>> 126ffe2 (trans_32 added)
            ptr = ptr->next;
        } else {
            printf("  Valid: 0\n");
            printf("  tag:  --\n");
        }
        printf("  ************\n");
    }
}

<<<<<<< HEAD
Cache *cacheNew(int s, int E) {

    unsigned long S = 1UL << ((unsigned long)s);
=======
/* Creates a new Cache and returns a pointer to it. If space could not
   be allocated, calls exit(1) */

Cache *cacheNew(int s, word_t E, int b, int t) {

>>>>>>> 126ffe2 (trans_32 added)
    Cache *C = calloc(1, sizeof(Cache));
    if (C == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }

<<<<<<< HEAD
    C->S = S;
=======
    word_t S = 1UL << ((word_t)s);
    word_t B = 1UL << ((word_t)b);

    C->s = (word_t)s;
    C->b = (word_t)b;
    C->t = (word_t)t;
    C->S = S;
    C->E = E;
    C->B = B;
>>>>>>> 126ffe2 (trans_32 added)

    C->Sets = malloc(S * sizeof(Set *));
    if (C->Sets == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }

<<<<<<< HEAD
    for (unsigned long i = 0; i < S; i++) {
        C->Sets[i] = setNew(E);
=======
    for (word_t i = 0; i < S; i++) {
        C->Sets[i] = setNew(C->E);
>>>>>>> 126ffe2 (trans_32 added)
    }

    return C;
}

<<<<<<< HEAD
=======
/* Frees a Set and all its members */

>>>>>>> 126ffe2 (trans_32 added)
void cacheFree(Cache *C) {
    if (C == NULL)
        return;

<<<<<<< HEAD
    for (unsigned long i = 0; i < C->S; i++) {
=======
    for (word_t i = 0; i < C->S; i++) {
>>>>>>> 126ffe2 (trans_32 added)
        setFree(C->Sets[i]);
    }

    free(C->Sets);
    free(C);
}

<<<<<<< HEAD
=======
/* A function used to pretty print a representation of a cache */

>>>>>>> 126ffe2 (trans_32 added)
void cachePrint(Cache *C) {

    if (C == NULL)
        return;

    printf("Puru's Cache Sim\n");
    printf(" S: %lu\n", C->S);
<<<<<<< HEAD
    printf(" E: %d\n", C->Sets[0]->E);
    printf(" hits: %lu\n", C->hits);
    printf(" misses: %lu\n", C->misses);
    printf(" evictions: %lu\n", C->evictions);
    printf(" dirty_evictions: %lu\n", C->dirty_evictions);
    printf("-------------------------------------------\n");

    for (unsigned long i = 0; i < C->S; i++) {
=======
    printf(" E: %lu\n", C->Sets[0]->E);
    printSummary(&C->stats);
    printf("-------------------------------------------\n");

    for (word_t i = 0; i < C->S; i++) {
>>>>>>> 126ffe2 (trans_32 added)
        setPrint(C->Sets[i], i);
    }
}

<<<<<<< HEAD
=======
/* A function that manages the the entire state of the
    cache based on the parsed operations */

void cacheProcess(Cache *C, char op, int vFlag, word_t addr) {

    word_t set_idx = (C->s != 0) ? (addr << C->t) >> (C->t + C->b) : 0;
    word_t tag_bits = addr >> (C->s + C->b);
    Set *set =
        C->Sets[set_idx]; // Get a pointer to the set specified by set_idx
    Line *prev = NULL;
    Line *curr = set->LRU;

    // Search for tag
    for (size_t i = 0; i < set->size; i++) {

        // HIT!

        if (curr->valid == 1 && curr->tag == tag_bits) {

            if (vFlag)
                printf(" - Hit!\n");

            C->stats.hits++;

            // If its a store operation
            // block has to be dirty
            if (!curr->dirty && op == 'S') {
                curr->dirty = 1;
                C->stats.dirty_bytes += C->B;
            }

            // Replace MRU
            setReplaceMRU(set, curr, prev);

            return;
        }

        prev = curr;
        curr = curr->next;
    }

    // No match founc => Miss.
    if (vFlag)
        printf(" - Miss!\n");

    // If store + miss then the byte is dirty.
    word_t is_dirty = (op == 'S') ? 1 : 0; // If store then dirty

    // Insert to MRU.
    // It's return value will indicate if an eviction has occured.

    int isEvict = setInsertMRU(set, tag_bits, &is_dirty);
    C->stats.dirty_bytes += (op == 'S') ? C->B : 0;

    if (isEvict) {
        if (vFlag)
            printf(" - Eviction!\n");

        // In the case of eviction, The value of is_dirty is
        // updated to the indicate if the bytes evisted were
        // dirty or not.
        C->stats.dirty_evictions += is_dirty * C->B;
        C->stats.dirty_bytes = C->stats.dirty_bytes - is_dirty * C->B;
        C->stats.evictions++;
    }

    C->stats.misses++;

#ifdef DEBUG_CACHE
    printf("set index: %lu, tag bits: %lx, ", set_idx, tag_bits);
    printf("dirty evicts: %lu, dirty bytes: %lu", C->stats.dirty_evictions,
           C->stats.dirty_bytes);
    printf("\n\n");
#endif

    return;
}

>>>>>>> 126ffe2 (trans_32 added)
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

<<<<<<< HEAD
    validate(&m, &s, &b, &E, tFile, &t);

    Cache *cache = cacheNew(s, E);

#ifdef DEGUB_CACHE
    cachePrint(cache);
#endif

    FILE *traceFile;

    char op;
    unsigned long addr;
=======
    /* Check if arguments passes are valic */
    validate(&m, &s, &b, &E, tFile, &t);

    Cache *cache = cacheNew(s, (word_t)E, b, t); /* Create a Cache */
    FILE *traceFile;

    char op;
    word_t addr;
>>>>>>> 126ffe2 (trans_32 added)
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

<<<<<<< HEAD
#ifdef DEBUG_FILE_READ
        printf("%c %lx,%d\n", op, addr, size);
#endif
    }

=======
        if (vFlag)
            printf("%c %lx,%d", op, addr, size);

        cacheProcess(cache, op, vFlag, addr);
    }

    printSummary(&cache->stats);

#ifdef DEBUG_CACHE
    cachePrint(cache);
#endif

>>>>>>> 126ffe2 (trans_32 added)
    fclose(traceFile);
    cacheFree(cache);

    return 0;
}

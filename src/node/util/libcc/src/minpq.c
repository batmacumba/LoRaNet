/*
 * Includes & Macros
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minpq.h"

#undef minpq_insert

/*
 * Data Structures
 */

struct MinPQ {
    char** keys;
    int n;
    int allocated;
    int max_key;
};

/**
 *  minpq_new():
 *      Initializes an empty minpq and returns a pointer to it.
 */
MinPQ *
minpq_new() {
    MinPQ *mpq = (MinPQ*) malloc(sizeof(MinPQ));
    if (!mpq) {
        fprintf(stderr, "minpq_new: cannot create new queue!\n");
        return NULL;
    }
    mpq -> n = 0;
    mpq -> allocated = 2;
    mpq -> keys = malloc(sizeof(char*) * 2);
    if (!mpq -> keys) {
        fprintf(stderr, "minpq_new: cannot allocate space for keys!\n");
        return NULL;
    }
    mpq -> keys[0] = NULL;
    mpq -> max_key = 0;
    return mpq;
}

/**
 *  minpq_isEmpty():
 *      Returns 1 if this priority queue is empty, 0 if it's not.
 */
int
minpq_isEmpty(MinPQ *mpq) {
    if (mpq -> n == 0) return 1;
    else return 0;
}

/**
 *  minpq_size():
 *      Returns the number of items on this priority queue.
 */
int
minpq_size(MinPQ *mpq) {
    return mpq -> n;
}

/**
 *  minpq_min():
 *      Returns the smallest key on this priority queue.
 */
char *
minpq_min(MinPQ *mpq) {
    if (minpq_isEmpty(mpq)) {
        fprintf(stderr, "minpq_min: queue is empty!\n");
        return NULL;
    }
    return mpq -> keys[1];
}

/**
 *  minpq_maxkey():
 *      Returns the length of the biggest key
 */
int
minpq_maxkey(MinPQ *mpq) {
    return mpq -> max_key;
}

/*******************************************************************************
 *                              Helper functions
 ******************************************************************************/

/**
 *  resize():
 *      Resizes the key array to the given capacity.
 */
static int
resize(MinPQ *mpq, int new_allocated) {
    /* size of the new block of memory */
    size_t arraySize = new_allocated * sizeof(char *);
    char **new_keys = NULL;
    /* new keys points to a block of memory the size we need */
    new_keys = (char**) realloc(mpq -> keys, arraySize);
    if (!new_keys) {
        fprintf(stderr, "resize: cannot resize minpq\n");
        return 1;
    }
    mpq -> keys = new_keys;
    mpq -> allocated = new_allocated;
    return 0;
}

/**
 *  exch():
 *      Swaps two elements on this priority queue.
 */
static void
exch(MinPQ *mpq, int i, int j) {
    char* swap = mpq -> keys[i];
    mpq -> keys[i] = mpq -> keys[j];
    mpq -> keys[j] = swap;
}

/**
 *  greater():
 *      @returns 1 if the second element is greater than the first.
 */
static int
greater(MinPQ *mpq, int i, int j) {
    return strcmp(mpq -> keys[i], mpq -> keys[j]) > 0;
}

/**
 *  swim():
 *      If the child is greater than the parent, swap them.
 */
static void
swim(MinPQ *mpq, int k) {
    while (k > 1 && greater(mpq, k / 2, k)) {
        exch(mpq, k, k / 2);
        k = k / 2;
    }
}

/**
 *  sink():
 *      Put this child where it belongs.
 */
static void
sink(MinPQ *mpq, int k) {
    while (2 * k <= mpq -> n) {
        int j = 2 * k;
        if (j < mpq -> n && greater(mpq, j, j + 1)) j++;
        if (!greater(mpq, k, j)) break;
        exch(mpq, k, j);
        k = j;
    }
}

/**
 *  isMinHeap():
 *      Makes sure this is a min heap.
 */
static int
isMinHeap(MinPQ *mpq, int k) {
    if (k > mpq -> n) return 1;
    int left = 2 * k;
    int right = 2 * k + 1;
    if (left <= mpq -> n && greater(mpq, k, left)) return 0;
    if (right <= mpq -> n && greater(mpq, k, right)) return 0;
    return isMinHeap(mpq, left) && isMinHeap(mpq, right);
}

/******************************************************************************/

/**
 *  minpq_insert():
 *      Inserts a key on this priority queue.
 *      @returns 0 if no error
 *      @returns 1 otherwise
 */
int
minpq_insert(MinPQ *mpq, char *key) {
    size_t keySize = sizeof(char) * strlen(key) + 1;
    if ((int) keySize > mpq -> max_key) mpq -> max_key = (int) keySize;
    /* if heap is full, double it's size */
    if (mpq -> n + 1 == mpq -> allocated) resize(mpq, mpq -> allocated * 2);
    /* allocates space on the next entry and copies the data */
    mpq -> keys[++mpq -> n] = malloc(keySize);
    memcpy(mpq -> keys[mpq -> n], key, keySize);
    swim(mpq, mpq -> n);
    return !isMinHeap(mpq, 1);
}

/**
 *  minpq_delMin():
 *      Removes and returns the smallest key on this priority queue.
 */
char *
minpq_delMin(MinPQ *mpq) {
    if (minpq_isEmpty(mpq)) {
        fprintf(stderr, "minpq_delMin: queue is empty!\n");
        return NULL;
    }
    char *min = mpq -> keys[1];
    exch(mpq, 1, mpq -> n--);
    sink(mpq, 1);
    mpq -> keys[mpq -> n + 1] = NULL;
    /* if we're at 25%, shrink mpq */
    if ((mpq -> n > 0) && (mpq -> n == (mpq -> allocated) / 4))
        resize(mpq, mpq -> allocated / 2);
    isMinHeap(mpq, 1);
    return min;
}

/**
 *  minpq_destroy():
 *      Destroys a priority queue, freeing all it's allocated memory.
 */
void minpq_destroy(MinPQ *mpq) {
    for (int i = 0; i < mpq -> allocated; i++)
        free(mpq -> keys[i]);
    free(mpq);
}

/**
 *  main():
 *      Unit test.
 */
int minpq_unitTest() {
    MinPQ *mpq = minpq_new();
    char *str = malloc(sizeof(char) * 32);
    gets(str);
    while (strcmp(str, "") != 0) {
        minpq_insert(mpq, str);
        gets(str);
    }
    
    while(!minpq_isEmpty(mpq)) {
        printf("size() = %d\n", minpq_size(mpq));
        printf("min() = %s\n", minpq_min(mpq));
        printf("delMin() = %s\n", minpq_delMin(mpq));
    }
    
    minpq_destroy(mpq);
    return 0;
    
}
 
 
 
 
 
 
 
 
 
 
 
 
 

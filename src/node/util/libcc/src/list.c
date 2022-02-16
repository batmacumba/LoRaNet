/*******************************************************************************
 *  list.c
 *      A generic list implementation.
 ******************************************************************************/

/* Includes & Macros */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

// #undef list_append
// #undef list_prepend
// #undef list_set

#define APPEND      0
#define PREPEND     1

/* Data Structure */
struct List {
    void **items;
    int allocated;                      // allocated storage capacity
    int n;                              // number of elements
    int zero;                           // index of the first element
};

/**
 *  list_new():
 *      Initializes an empty list and returns a pointer to it.
 *      // TODO: copy vs no-copy as argument
 */
List *
list_new() 
{
    List *l = (List *) malloc(sizeof(List));
    if (!l) {
        fprintf(stderr, "list_new: cannot create new list!\n");
        return NULL;
    }
    l -> items = NULL;
    l -> n = 0;
    l -> allocated = 0;
    l -> zero = 0;
    return l;
}

/**
 *  list_resize():
 *      Resizes the list to the given capacity.
 */
static int
list_resize(List *l, int new_allocated, int flag) 
{
    void **new_items = (void **) calloc((size_t) new_allocated, sizeof(void *));
    
    if (flag == APPEND)
        memcpy(new_items, l -> items, l -> allocated * sizeof(void *));
    /* prepend allocates and copies to the middle of the new block */
    else if (flag == PREPEND) {
        memcpy(new_items + (new_allocated / 2), l -> items, l -> allocated * sizeof(void *));
        /* offsets the "zero element" on the list */
        l -> zero = new_allocated / 2;
    }
    /* error checking and variable updating */
    if (!new_items) {
        fprintf(stderr, "list_resize: cannot resize list\n");
        return -1;
    }
    // TODO: free no ponteiro l -> items anterior?
    l -> items = new_items;
    l -> allocated = new_allocated;

    return 0;
}

/**
 *  list_mem_handler():
 *      Checks if the list needs to shrink or grow and acts accordingly.
 */
static int
list_mem_handler(List *l, int flag) 
{
    int handler;
    /* full or empty */
    if (l -> n == l -> allocated) {
        if (l -> allocated == 0) handler = list_resize(l, 1, APPEND);
        else handler = list_resize(l, 2 * l -> allocated, flag);
    }
    /* very sparse - caution with division by 0 */
    else if (l -> n > 0 && l -> allocated / l -> n >= 4) {
        // TODO: funciona isso?
        handler = list_resize(l, l -> allocated / 2, flag);
    }
    /* no empty space on the beggining */
    else if (l -> items[0] != NULL && flag == PREPEND) {
        handler = list_resize(l, 2 * l -> allocated, flag);
    }
    /* no resize needed, move along */
    else {
        // puts("no resize");
        handler = 0;
    }
    /* error? */
    if (handler < 0)
        fprintf(stderr, "list_mem_handler: error resizing list\n");
    return handler;
    
}

/**
 *  list_append():
 *      Adds an item to the end of the list.
 */
int
list_append(List *l, void *data, size_t dataSize) 
{
    if (list_mem_handler(l, APPEND) < 0) {
        fprintf(stderr, "list_append: error handling memory\n");
        return -1;
    }
    /* allocates space on the heap and copies the data */
    l -> items[l -> zero + l -> n] = malloc(dataSize);
    // TODO: malloc error check
    memcpy(l -> items[l -> zero + l -> n], data, dataSize);
    l -> n++;
    return 0;
}

/**
 *  list_prepend():
 *      Adds an item to the beggining of the list.
 */
int list_prepend(List *l, void *data, size_t dataSize) 
{
    if (list_mem_handler(l, PREPEND) < 0) {
        fprintf(stderr, "list_prepend: error handling memory\n");
        return -1;
    }
    /* allocates space on the heap and copies the data */
    if (l -> zero > 0) l -> zero--;
    l -> items[l -> zero] = malloc(dataSize);
    // TODO: malloc error check
    memcpy(l -> items[l -> zero], data, dataSize);
    l -> n++;
  
    return 0;
}

static int
index_handler(List *l, int i)
{
    if (i > l -> n - 1) return -1;
    /* negative index returns n - i item */
    if (i < 0) {
        i += l -> n;
        if (i < 0) return -1;
    }
    return (l -> zero + i);
}

/**
 *  list_get():
 *      Returns the item at the given index.
 */
void *
list_get(List *l, int i) 
{
    i = index_handler(l, i);
    if (i < 0) {
        fprintf(stderr, "list_get: index out of range\n");
        return NULL;
    }
    return l -> items[i];
}

/**
 *  list_set():
 *      Sets the item at the given index to the given value.
 */
int
list_set(List *l, int i, void *data, size_t dataSize) 
{
    i = index_handler(l, i);
    if (i < 0) {
        fprintf(stderr, "list_set: index out of range\n");
        return -1;
    }
    
    if (l -> items[i]) free(l -> items[i]);
    l -> items[i] = malloc(dataSize);
    if (!l -> items[i]) {
        fprintf(stderr, "list_set: cannot allocate space\n");
        return -1;
    }
    /* finally copies the data and returns */
    memcpy(l -> items[i], data, dataSize);
    return 0;
}

int
list_del(List *l, int i) 
{
    // TODO: mem_handler checar se precisa resize
    i = index_handler(l, i);
    if (i < 0) {
        fprintf(stderr, "list_del: index out of range\n");
        return -1;
    }
    printf("del %d\n", i);
    if (l -> items[i]) {
        free(l -> items[i]);
        l -> items[i] = NULL;
        // TODO: bug: garantir que todos os ponteiros vazios estão NULL de fato
        /* covers holes in the array */
        if (i < l -> zero + l -> n - 1 && l -> items[i + 1] != NULL) {
            while (i < l -> zero + l -> n - 1) {
                l -> items[i] = l -> items[i + 1];
                i++;
            }
        }
    }
    l -> n--;
    return 0;
}

// TODO: doc, remove mais de um? retorna o que?
int
list_remove(List *l, void *data, int (compare)(void *, void *)) 
{
    // TODO: mem_handler checar se precisa resize
    for (int i = l -> zero; i < l -> zero + l -> n; i++)
        if (compare(data, l -> items[i]) == 0) {
            list_del(l, i - l -> zero);
            return 1;
        }
    return 0;
}

int
list_contains(List *l, void *data, int (compare)(void *, void *)) 
{
    for (int i = l -> zero; i < l -> zero + l -> n; i++)
        if (compare(data, l -> items[i]) == 0) return 1;
    return 0;
}

int 
list_is_empty(List *l)
{
    return (l -> n == 0);
}

int
list_length(List *l)
{
    return (l -> n);
}

// #define list_append(l, data) list_append(l, data, sizeof(data))
// #define list_prepend(l, data) list_prepend(l, data, sizeof(data))
// #define list_set(l, i, data) list_set(l, i, data, sizeof(data))

int
str_compare(void *a, void *b)
{
    char *str_a = (char *) a;
    char *str_b = (char *) b;
    return strcmp(str_a, str_b);
}

/* Testes */
int
main(int argc, char **argv)
{
    List *l = list_new();
    printf("--------- list_is_empty ----------\n");
    printf("%d\n", list_is_empty(l));
    printf("--------- list_append(\"teste2\") ----------\n");
    list_append(l, "teste2", strlen("teste2"));
    for (int i = 0; i < list_length(l); i++) printf("%s\n", list_get(l, i));
    printf("--------- list_append(\"teste3\") ----------\n");
    list_append(l, "teste3", strlen("teste3"));
    for (int i = 0; i < list_length(l); i++) printf("%s\n", list_get(l, i));
    // printf("--------- list_prepend(\"teste1\") ----------\n");
    // list_prepend(l, "teste1", strlen("teste1"));
    // for (int i = 0; i < list_length(l); i++) printf("%s\n", list_get(l, i));
    printf("--------- list_contains ----------\n");
    char *str = list_get(l, 0);    
    printf("list_contains %s ? %d\n", str, list_contains(l, str, &str_compare));
    str = "testeXXX";    
    printf("list_contains %s ? %d\n", str, list_contains(l, str, &str_compare));
    printf("--------- list_del(1) ----------\n");
    list_del(l, 1);
    for (int i = 0; i < list_length(l); i++) printf("%s\n", list_get(l, i));
    printf("--------- list_remove(\"testeX\") ----------\n");
    list_remove(l, "testeX", &str_compare);
    for (int i = 0; i < list_length(l); i++) printf("%s\n", list_get(l, i));
    printf("--------- list_remove(\"teste1\") ----------\n");
    list_remove(l, "teste1", &str_compare);
    for (int i = 0; i < list_length(l); i++) printf("%s\n", list_get(l, i));
    printf("--------- list_remove(\"teste2\") ----------\n");
    list_remove(l, "teste2", &str_compare);
    for (int i = 0; i < list_length(l); i++) printf("%s\n", list_get(l, i));
    printf("--------- list_append(\"teste2\") ----------\n");
    list_append(l, "teste2", strlen("teste2"));
    for (int i = 0; i < list_length(l); i++) printf("%s\n", list_get(l, i));
    return 0;
}

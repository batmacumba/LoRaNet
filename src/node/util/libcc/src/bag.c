/*******************************************************************************
 bag.c
    Implementation of a bag using a linked list.
*******************************************************************************/

/**
 * Includes & Macros
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/error.h"
#include "bag.h"

#undef bag_add

/**
 * Data Structures
 */
typedef struct Item {
    void *data;
    struct Item *next;
} Item;

struct Bag {
    Item *top;
    int size;
};

/**
 * bag_new - initializes an empty bag
 */
Bag *
bag_new() {
    Bag *b = (Bag*) malloc(sizeof(Bag));
    if (!b) {
        fprintf(stderr, "bag_new: cannot create new bag!\n");
        return NULL;
    }
    b -> top = NULL;
    b -> size = 0;
    return b;
}

/**
 * bag_isEmpty - checks if the first element on the list is null
 * @return: 1 if empty, 0 otherwise
 */
int
bag_isEmpty(Bag *b) {
    if (b -> top == NULL) return 1;
    else return 0;
}

/**
 * bag_size - returns the size of the bag
 * @return: number of elements
 */
int
bag_size(Bag *b) {
    return b -> size;
}

/**
 * bag_newItem - allocates an empty item
 * @dataSize: number of bytes to be allocated
 * @return: pointer to new item
 */
Item *
bag_newItem(size_t dataSize) {
    Item *i = (Item*) malloc(sizeof(Item));
    if (!i) {
        fprintf(stderr, "bag_newItem: cannot malloc new item!\n");
        return NULL;
    }
    
    i -> data = malloc(dataSize);
    if (!i -> data) {
        fprintf(stderr, "bag_newItem: cannot malloc space for data!\n");
        return NULL;
    }
    
    i -> next = NULL;
    return i;
}

/**
 * bag_add - copies the data and adds it to the top the list
 * @b:
 * @data:
 * @dataSize:
 */
int
bag_add(Bag *b, void *data, size_t dataSize) {
    Item *i = bag_newItem(dataSize);
    if (i == NULL) {
        fprintf(stderr, "bag_add: cannot create new item!\n");
        return 1;
    }
    memcpy(i -> data, data, dataSize);
    i -> next = b -> top;
    b -> top = i;
    b -> size++;
    return 0;
}

/*
 * bag_iterator():
 * Copies every item in the bag and returns an array of items.
 */
void **bag_iterator(Bag *b) {
    if (bag_isEmpty(b) == 1) {
        fprintf(stderr, "bag_iterator: bag is empty!\n");
        return NULL;
    }
    void **items = (malloc(sizeof(void*) * b -> size));
    Item *i = b -> top;
    int k = 0;
    while (k < b -> size) {
        items[k] = i -> data;
        i = i -> next;
        k++;
    }
    return items;
}

/*
 * bag_destroy():
 * Destroys a bag, freeing all it's allocated memory.
 */
void bag_destroy(Bag *b) {
    if (!bag_isEmpty(b)) {
        Item *i = b -> top;
        while (i != NULL) {
            /* top of the bag points to next element */
            b -> top = i -> next;
            free(i -> data);
            free(i);
            i = b -> top;
        }
    }
    free(b);
}

/*
 * bag_unitTest():
 * Unit test.
 */
void bag_unitTest() {
    puts("bag_unitTest()");
    Bag *bag = bag_new();
    int tmp1 = 1;
    int tmp2 = 2;
    int tmp3 = 3;
    bag_add(bag, &tmp1, sizeof(int));
    bag_add(bag, &tmp2, sizeof(int));
    bag_add(bag, &tmp3, sizeof(int));

    int **bag_items = (int**) bag_iterator(bag);
    for (int i = 0; i < bag_size(bag); i++) {
        printf("item %d = %d\n", i, *bag_items[i]);
    }
    puts("");
    bag_destroy(bag);
}


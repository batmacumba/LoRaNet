/*******************************************************************************
 * Bag: collection of non-unique elements
 * Dependencies: stdio.h, stdlib.h, string.h
 * Compilation:
     $ make bag.o
     $ gcc file.c bag.o -o file
 * Include this header in your file
 ******************************************************************************/

#ifndef _BAG_H_
#define _BAG_H_

typedef struct Bag Bag;

/**
 * Available operations
 */
Bag *bag_new();                                         // constructor
int bag_isEmpty(Bag *b);                            // is bag empty?
int bag_size(Bag *b);                                   // returns n of elements
int bag_add(Bag *b, void *data, size_t dataSize);       // adds an item
void **bag_iterator(Bag *b);                            // iterator
void bag_destroy(Bag *b);                               // frees memory
void bag_unitTest();                                    // test

/* To avoid having to pass sizeof(x) as argument every time we add something */
//#define bag_add(b, data) bag_add(b, data, strlen(data))

#endif /* _BAG_H_ */

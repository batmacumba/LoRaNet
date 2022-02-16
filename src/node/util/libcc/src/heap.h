/*******************************************************************************
 * Heap: dinamically resized max-heap
 * Dependencies: stdio.h, stdlib.h, string.h
 * Compilation:
     $ make heap.o
     $ gcc file.c heap.o -o file
 * Include this header in your file
 ******************************************************************************/

#ifndef _HEAP_H_
#define _HEAP_H_

typedef struct Heap Heap;

/*
 * Available operations
 */
Heap *heap_new();                                                   //construct
int heap_insert(Heap *v, void *data, size_t dataSize,               //inserts
                int (*comparer)(void *a, void *b));
int heap_extractMax(Heap *v, int (*comparer)(void *a, void *b));   //extracts max
void *heap_peek(Heap *v);                                           //accesses max
void *heap_at(Heap *v, int index);                                  //accesses
int heap_getSize(Heap *v);
int heap_getCapacity(Heap *v);
void heap_destroy(Heap *v);                                         //free
void heap_unitTest();                                               //test

/* Let's avoid having to pass sizeof(x) as argument every time we add something.
 * Instead write heap_insert(v, data, comparer) and this macro will expand sizeof.
 */
#define heap_insert(v, data, comparer) heap_insert(v, data, sizeof(data), comparer)

#endif /* _HEAP_H_ */

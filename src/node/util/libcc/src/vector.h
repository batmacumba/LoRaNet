/*******************************************************************************
 * Vector: dinamically resized array (C++-like vector)
 * Dependencies: stdio.h, stdlib.h, string.h
 * Compilation:
     $ make vector.o
     $ gcc file.c vector.o -o file
 * Include this header in your file
 ******************************************************************************/

#ifndef _VECTOR_H_
#define _VECTOR_H_

typedef struct Vector Vector;

/*
 * Available operations
 */
Vector *vector_new();                                               // construct
int vector_pushBack(Vector *v, void *new_data, size_t dataSize);    // pushes
int vector_popBack(Vector *v);                                      // pops
void *vector_at(Vector *v, int index);                              // accesses
int vector_isEmpty(Vector *v);
int vector_getSize(Vector *v);
int vector_getCapacity(Vector *v);
void vector_destroy(Vector *v);                                     // free
void vector_unitTest();                                             // test

/* Let's avoid having to pass sizeof(x) as argument every time we add something.
 * Instead write vector_pushBack(v, data) and this macro will expand sizeof.
 */
#define vector_pushBack(v, data) vector_pushBack(v, data, sizeof(data))

#endif /* _VECTOR_H_ */

/*******************************************************************************
 * Stack: LIFO collection of non-unique elements
 * Dependencies: stdio.h, stdlib.h, string.h
 * Compilation:
     $ make stack.o
     $ gcc file.c stack.o -o file
 * Include this header in your file
 ******************************************************************************/

#ifndef _STACK_H_
#define _STACK_H_

typedef struct Stack Stack;

/*
 * Available operations
 */
Stack *stack_new();                                         // constructor
int stack_isEmpty(Stack *s);                                // is stack empty?
int stack_size(Stack *s);                                   // return stck size
int stack_push(Stack *s, void *new_data, size_t dataSize);  // pushes an item
void *stack_pop(Stack *s);                                  // pops an item
void *stack_peek(Stack *s);                                 // returns top item
void **stack_iterator(Stack *s);                            // iterator
void stack_unitTest();                                      // test
void stack_destroy(Stack *s);                               // frees memory

/* Let's avoid having to pass sizeof(x) as argument every time we add something.
 * Instead write stack_push(s, data) and this macro will expand sizeof.
 */
#define stack_push(s, data) stack_push(s, data, sizeof(data))

#endif /* _STACK_H_ */

/*
 * Includes & Macros
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#undef stack_push

/*
 * Data Structures
 */
typedef struct Item {
    void *data;
    struct Item *next;
} Item;

struct Stack {
    Item *top;
    int size;
};

/*
 * stack_new():
 * Initializes an empty stack and returns a pointer to it.
 */
Stack *stack_new() {
    Stack *s = (Stack*) malloc(sizeof(Stack));
    if (!s) {
        fprintf(stderr, "stack_new: Cannot create new stack!\n");
        return NULL;
    }
    s -> top = NULL;
    s -> size = 0;
    return s;
}

/*
 * stack_isEmpty():
 * Returns 1 if this stack is empty, 0 if it's not.
 */
int stack_isEmpty(Stack *s) {
    if (s -> top == NULL) return 1;
    else return 0;
}

/*
 * stack_size():
 * Returns the number of items on this stack.
 */
int stack_size(Stack *s) {
    return s -> size;
}

/*
 * S_Item_new():
 * Initializes an empty Item.
 */
Item *S_Item_new(size_t dataSize) {
    Item *i = (Item*) malloc(sizeof(Item));
    if (!i) {
        fprintf(stderr, "S_Item_new: cannot malloc new item!\n");
        return NULL;
    }
    i -> data = malloc(dataSize);
    if (!i -> data) {
        fprintf(stderr, "S_Item_new: cannot malloc space for data!\n");
        return NULL;
    }
    i -> next = NULL;
    return i;
}

/*
 * stack_push():
 * Copies the data and adds the item to the stack.
 * Returns 0 if item was succesfully pushed or 1 otherwise.
 */
int stack_push(Stack *s, void *data, size_t dataSize) {
    Item *i = S_Item_new(dataSize);
    if (i == NULL) {
        fprintf(stderr, "stack_push: cannot create new item!\n");
        return 1;
    }
    memcpy(i -> data, data, dataSize);
    i -> next = s -> top;
    s -> top = i;
    s -> size++;
    return 0;
}

/*
 * stack_pop():
 * Removes the item from the top of the stack and returns it.
 */
void *stack_pop(Stack *s) {
    if (stack_isEmpty(s)) {
        fprintf(stderr, "stack_pop: stack is empty!\n");
        return NULL;
    }
    void *data = s -> top -> data;
    s -> top = s -> top -> next;
    s -> size--;
    return data;
}

/*
 * stack_peek():
 * Returns the item on the top of the stack without removing it.
 */
void *stack_peek(Stack *s) {
    if (stack_isEmpty(s)) {
        fprintf(stderr, "stack_peek: stack is empty!\n");
        return NULL;
    }
    return s -> top -> data;
}

/*
 * stack_iterator():
 * Copies all the items on the stack and returns an array.
 */
void **stack_iterator(Stack *s) {
    if (stack_isEmpty(s)) {
        fprintf(stderr, "stack_iterator: stack is empty!\n");
        return NULL;
    }
    void **items = (malloc(sizeof(void*) * s -> size));
    Item *i = s -> top;
    int k = 0;
    while (k < s -> size) {
        items[k] = i -> data;
        i = i -> next;
        k++;
    }
    return items;
}

/*
 * stack_destroy():
 * Destroys a stack, freeing all it's allocated memory.
 */
void stack_destroy(Stack *s) {
    if (!stack_isEmpty(s)) {
        Item *i = s -> top;
        while (i != NULL) {
            /* top of the stack points to next element */
            s -> top = i -> next;
            free(i -> data);
            free(i);
            i = s -> top;
        }
    }
    free(s);
}

/*
 * stack_unitTest():
 * Unit test.
 */
void stack_unitTest() {
    puts("stack_unitTest()");
    Stack *stack = stack_new();
    stack_push(stack, "teste3", sizeof("teste3"));
    stack_push(stack, "teste2", sizeof("teste2"));
    stack_push(stack, "teste1", sizeof("teste1"));
    printf("retorno = %s\n", stack_pop(stack));
    printf("retorno = %s\n", stack_pop(stack));
    printf("retorno = %s\n", stack_pop(stack));
    puts("");
    stack_destroy(stack);
}

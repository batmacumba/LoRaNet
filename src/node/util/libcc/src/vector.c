/*
 * Includes & Macros
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

#undef vector_pushBack

/*
 * Data Structures
 */
struct Vector {
    void** items;
    int capacity;   //allocated storage capacity
    int size;       //number of inserted elements
};

/*
 * vector_new():
 * Initializes an empty vector and returns a pointer to it.
 */
Vector *vector_new()
{
    Vector *v = (Vector*) malloc(sizeof(Vector));
    if (!v) {
        fprintf(stderr, "vector_new: Cannot create new vector!\n");
        return NULL;
    }
    v->items = (void**)malloc(sizeof(void*));
    v->size = 0;
    v->capacity = 1;
    return v;
}

/*
 * vector_getSize():
 * Returns size of vector
 */
int vector_getSize(Vector *v) {
    return v->size;
}

/*
 * vector_getCapacity():
 * Returns capacity of vector
 */
int vector_getCapacity(Vector *v) {
    return v->capacity;
}

/*
 * vector_resize():
 * Resizes the vector so its capacity is new_size.
 * Returns 0 if item was succesfully resized and 1 otherwise.
 * If resizing is not sucessfull, vector remains unaltered.
 */
int vector_resize(Vector *v, int new_size)
{
    void** items_tmp = (void**)realloc(v->items, new_size*sizeof(void*));
    if(!items_tmp) {
        fprintf(stderr, "vector_resize: Cannot resize vector!\n");
        return 1;
    }
    v->items = items_tmp;
    v->capacity = new_size;
    return 0;
}

/*
 *vector_at();
 * Returns element at given index
 * If is out of range, returns NULL
 */
void *vector_at(Vector *v, int index) {
    if (index >= v->size || index < 0) {
        fprintf(stderr, "vector_at: index out of range!\n");
        return NULL;
    }
    return v->items[index];
}

/*
 * vector_isEmpty();
 * Returns 1 if vector is empty and 0 otherwise
 */
int vector_isEmpty(Vector *v) {
    return (v->size == 0);
}

/*
 * vector_pushBack():
 * Adds the item to the end of the vector.
 * Returns 0 if item was succesfully added and 1 otherwise.
 */
int vector_pushBack(Vector *v, void *data, size_t dataSize) {
    if(v->size == v->capacity) {
        if (vector_resize(v, 2*v->capacity)) {
            fprintf(stderr, "vector_pushBack: Cannot append to vector!\n");
            return 1;
        }
    }
    v->items[v->size] = malloc(dataSize);
    memcpy(v->items[v->size++], data, dataSize);
    return 0;
}

/*
 * vector_popBack():
 * Removes the item at the end of the vector.
 * Returns 0 if item was succesfully removed and 1 otherwise.
 */
int vector_popBack(Vector *v) {
    if(v->size == 0) {
        fprintf(stderr, "vector_popBack: vector is already empty!\n");
        return 1;
    }
    free(v->items[v->size - 1]);
    v->size--;
    return 0;
}

/*
 * vector_destroy():
 * Destroys a vector, freeing all the allocated memory it is using.
 */
void vector_destroy(Vector *v) {
    for(int i = 0; i < v->size; i++)
        free(v->items[i]);
    free(v->items);
    free(v);
    v = NULL;
}

/*
 * Unit test.
 */
void vector_unitTest() {
    puts("vector_unitTest()");
    Vector *v = vector_new();

    int *tmp1 = (int*)malloc(sizeof(int));
    int *tmp2 = (int*)malloc(sizeof(int));
    int *tmp3 = (int*)malloc(sizeof(int));
    int *tmp4 = (int*)malloc(sizeof(int));
    int *tmp5 = (int*)malloc(sizeof(int));

    *tmp1 = 1;
    *tmp2 = 2;
    *tmp3 = 3;
    *tmp4 = 4;
    *tmp5 = 5;

    vector_pushBack(v, tmp1, sizeof(tmp1));
    vector_pushBack(v, tmp2, sizeof(tmp2));
    vector_pushBack(v, tmp3, sizeof(tmp3));
    vector_pushBack(v, tmp4, sizeof(tmp4));
    vector_pushBack(v, tmp5, sizeof(tmp5));

    printf("After 5 pushBack calls: \n");
    printf("Size: %d\n", vector_getSize(v));
    printf("Capacity: %d\n", vector_getCapacity(v));

    printf("Vector items: [");
    printf("%d", *(int*)(vector_at(v, 0)));
    for(int i = 1; i < vector_getSize(v); i++) {
        printf(", %d", *(int*)(vector_at(v, i)));
    }
    printf("]\n");

    vector_popBack(v);
    vector_popBack(v);

    printf("\nAfter 2 popBack calls: \n");
    printf("Size: %d\n", vector_getSize(v));
    printf("Capacity: %d\n", vector_getCapacity(v));

    printf("Vector items: [");
    printf("%d", *(int*)(vector_at(v, 0)));
    for(int i = 1; i < vector_getSize(v); i++)
        printf(", %d", *(int*)(vector_at(v, i)));
    printf("]\n");
    puts("");
    vector_destroy(v);
}

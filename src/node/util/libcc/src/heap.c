/*
 * Includes & Macros
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"

#undef heap_insert

/*
 * Data Structures
 */
struct Heap {
    void** items;
    int capacity;   //allocated storage capacity
    int size;       //number of inserted elements
};

/*
 * heap_new():
 * Initializes an empty heap and returns a pointer to it.
 */
Heap *heap_new()
{
    Heap *v = (Heap*) malloc(sizeof(Heap));
    if (!v) {
        fprintf(stderr, "heap_new: Cannot create new heap!\n");
        return NULL;
    }
    v->items = (void**)malloc(2*sizeof(void*));
    v->size = 0;
    v->capacity = 1;
    return v;
}

/*
 * heap_resize():
 * Resizes the heap so its capacity is new_size.
 * Returns 0 if heap was succesfully resized and 1 otherwise.
 * If resizing is not sucessfull, heap remains unaltered.
 */
int heap_resize(Heap *v, int new_size)
{
    void** items_tmp = (void**)realloc(v->items, new_size*sizeof(void*));
    if(!items_tmp) {
        fprintf(stderr, "heap_resize: Cannot resize heap!\n");
        return 1;
    }
    v->items = items_tmp;
    v->capacity = new_size;
    return 0;
}

/*
 * heap_swap():
 * swaps element at child_index with element at parent_index
 */
void heap_swap(Heap *v, int child_index, int parent_index) {
        void *tmp_pointer = v->items[child_index];
        v->items[child_index] = v->items[parent_index];
        v->items[parent_index] = tmp_pointer;
}

/*
 * heap_heapifyUp():
 * standard heap algorithm
 */
void heap_heapifyUp(Heap *v, int (*comparer)(void *a, void *b)) {
    int child_index = v->size - 1;
    int parent_index = (child_index - 1)/2;
    while(parent_index >= 0 && comparer(v->items[child_index], v->items[parent_index]) == 1) {
        heap_swap(v, child_index, parent_index);
        child_index = parent_index;
        parent_index = (child_index - 1)/2;
    }
}

/*
* heap_insert():
* Adds item to heap.
* Returns 0 if item was succesfully added and 1 otherwise.
*/
int heap_insert(Heap *v, void *data, size_t dataSize, int (*comparer)(void *a, void *b)) {
        if(v->size == v->capacity) {
            if (heap_resize(v, 2*v->capacity)) {
                fprintf(stderr, "heap_insert: Cannot append to heap!\n");
                return 1;
            }
        }
        v->items[v->size] = malloc(dataSize);
        memcpy(v->items[v->size++], data, dataSize);
        heap_heapifyUp(v, comparer);
        return 0;
}

/*
 * heap_heapifyDown():
 * standard heap algorithm
 */
void heap_heapifyDown(Heap *v, int (*comparer)(void *a, void *b)) {
        int parent_index = 0;
        int left_child = 2*parent_index + 1;
        int right_child = 2*parent_index + 2;
        while(parent_index < v->size) {
                if(right_child < v->size && comparer(v->items[right_child], v->items[parent_index]) == 1 &&
                comparer(v->items[right_child], v->items[left_child]) == 1) {
                        heap_swap(v, right_child, parent_index);
                        parent_index = right_child;
                        left_child = 2*parent_index + 1;
                        right_child = 2*parent_index + 2;
                }
                else if (left_child < v->size && comparer(v->items[left_child], v->items[parent_index]) == 1) {
                        heap_swap(v, left_child, parent_index);
                        parent_index = left_child;
                        left_child = 2*parent_index + 1;
                        right_child = 2*parent_index + 2;
                }
                else break;
        }
}

/*
 * heap_extract():
 * Removes a maximum element from the heap
 * Returns 0 if item was succesfully removed and 1 otherwise.
 */
int heap_extractMax(Heap *v, int (*comparer)(void *a, void *b)) {
    if(v->size == 0) {
        fprintf(stderr, "heap_extractMax: heap is already empty!\n");
        return 1;
    }
    heap_swap(v, v->size - 1, 0);
    free(v->items[v->size - 1]);
    v->size--;
    heap_heapifyDown(v, comparer);
    return 0;
}

/*
 * heap_peek():
 * Returns a maximum element from the heap
 */
void *heap_peek(Heap *v) {
    if (v->size == 0) {
        fprintf(stderr, "heap_peek: heap is empty!\n");
        return NULL;
    }
    return v->items[0];
}

/*
 * heap_at():
 * Returns element at index.
 * If index is out of range, returns NULL.
 */
void *heap_at(Heap *v, int index) {
    if (index >= v->size || index < 0) {
        fprintf(stderr, "heap_at: index out of range!\n");
        return NULL;
    }
    return v->items[index];
}

/*
 * heap_getSize():
 * Returns size of heap
 */
int heap_getSize(Heap *v) {
    return v->size;
}

/*
 * heap_getCapacity():
 * Returns capacity of heap
 */
int heap_getCapacity(Heap *v) {
    return v->capacity;
}

/*
 * heap_destroy():
 * Destroys a heap, freeing all the allocated memory it is using.
 */
void heap_destroy(Heap *v) {
    for(int i = 0; i < v->size; i++)
        free(v->items[i]);
    free(v->items);
    free(v);
    v = NULL;
}

int heap_testComparer(void *a, void *b) {
        int a2 = *(int*)a;
        int b2 = *(int*)b;
        if(a2 > b2) return 1;
        else if (a2 < b2) return -1;
        return 0;
}

/*
 * Unit test.
 */
void heap_unitTest() {
    puts("heap_unitTest()");
    Heap *v = heap_new();

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

    heap_insert(v, tmp1, sizeof(tmp1), heap_testComparer);
    heap_insert(v, tmp2, sizeof(tmp2), heap_testComparer);
    heap_insert(v, tmp3, sizeof(tmp3), heap_testComparer);
    heap_insert(v, tmp4, sizeof(tmp4), heap_testComparer);
    heap_insert(v, tmp5, sizeof(tmp5), heap_testComparer);

    printf("After 5 insert calls: \n");
    printf("Size: %d\n", heap_getSize(v));
    printf("Capacity: %d\n", heap_getCapacity(v));

    printf("Heap items: [");
    printf("%d", *(int*)(heap_at(v, 0)));
    for(int i = 1; i < heap_getSize(v); i++) {
        printf(", %d", *(int*)(heap_at(v, i)));
    }
    printf("]\n");

    printf("Max element: %d\n", *(int*)(heap_peek(v)));
    heap_extractMax(v, heap_testComparer);
    printf("Extracted max element\n");
    printf("Max element: %d\n", *(int*)(heap_peek(v)));
    heap_extractMax(v, heap_testComparer);
    printf("Extracted max element\n");
    printf("Max element: %d\n", *(int*)(heap_peek(v)));
    heap_extractMax(v, heap_testComparer);
    printf("Extracted max element\n");

    printf("\nAfter 3 extractMax calls: \n");
    printf("Size: %d\n", heap_getSize(v));
    printf("Capacity: %d\n", heap_getCapacity(v));

    printf("Heap items: [");
    printf("%d", *(int*)(heap_at(v, 0)));
    for(int i = 1; i < heap_getSize(v); i++)
        printf(", %d", *(int*)(heap_at(v, i)));
    printf("]\n");
    puts("");
    heap_destroy(v);
}

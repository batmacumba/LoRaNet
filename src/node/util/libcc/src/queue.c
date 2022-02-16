/*
 * Includes & Macros
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#undef queue_enqueue

/*
 * Data Structures
 */
typedef struct Item {
    void *data;
    struct Item *next;
} Item;

struct Queue {
    Item *first;
    Item *last;
    int size;
};

/*
 * queue_new():
 * Initializes an empty queue and returns a pointer to it.
 */
Queue *queue_new() {
    Queue *q = (Queue*) malloc(sizeof(Queue));
    if (!q) {
        fprintf(stderr, "queue_new: cannot create new queue!\n");
        return NULL;
    }
    q -> first = NULL;
    q -> last = NULL;
    q -> size = 0;
    return q;
}

/*
 * queue_isEmpty():
 * Returns 1 if this queue is empty, 0 if it's not.
 */
int queue_isEmpty(Queue *q) {
    if (q -> first == NULL) return 1;
    else return 0;
}

/*
 * queue_size():
 * Returns the number of items on this queue.
 */
int queue_size(Queue *q) {
    return q -> size;
}

/*
 * Q_Item_new():
 * Initializes an empty Item.
 */
Item *Q_Item_new(size_t dataSize) {
    Item *i = (Item*) malloc(sizeof(Item));
    if (!i) {
        fprintf(stderr, "Q_Item_new: cannot malloc new item!\n");
        return NULL;
    }
    i -> data = malloc(dataSize);
    if (!i -> data) {
        fprintf(stderr, "Q_Item_new: cannot malloc space for data!\n");
        return NULL;
    }
    i -> next = NULL;
    return i;
}

/*
 * queue_enqueue():
 * Copies the data and adds the item to the end of the queue.
 * Returns 0 if item was succesfully enqueued or 1 otherwise.
 */
int queue_enqueue(Queue *q, void *data, size_t dataSize) {
    Item *old_last = q -> last;
    q -> last = Q_Item_new(dataSize);
    if (q -> last == NULL) {
        fprintf(stderr, "queue_enqueue: cannot create new item!\n");
        /* returns q -> last to it's previous address */
        q -> last = old_last;
        return 1;
    }
    memcpy(q -> last -> data, data, dataSize);
    if (queue_isEmpty(q) == 1) q -> first = q -> last;
    else old_last -> next = q -> last;
    q -> size++;
    return 0;
}

/*
 * queue_dequeue():
 * Removes the item from the first spot and returns it.
 */
void *queue_dequeue(Queue *q) {
    if (queue_isEmpty(q) == 1) {
        fprintf(stderr, "queue_dequeue: queue is empty!\n");
        return NULL;
    }
    void *data = q -> first -> data;
    q -> first = q -> first -> next;
    q -> size--;
    if (queue_isEmpty(q) == 1) q -> last = NULL;
    return data;
}

/*
 * queue_peek():
 * Returns the item on the first spot of the queue w/o removing
 */
void *queue_peek(Queue *q) {
    if (queue_isEmpty(q) == 1) {
        fprintf(stderr, "queue_peek: queue is empty!\n");
        return NULL;
    }
    return q -> first -> data;
}

/*
 * queue_iterator():
 * Copies all the items on the queue and returns an array.
 */
void **queue_iterator(Queue *q) {
    if (queue_isEmpty(q)) {
        fprintf(stderr, "queue_iterator: queue is empty!\n");
        return NULL;
    }
    void **items = (malloc(sizeof(void*) * q -> size));
    Item *i = q -> first;
    int k = 0;
    while (k < q -> size) {
        items[k] = i -> data;
        i = i -> next;
        k++;
    }
    return items;
}

/*
 * queue_destroy():
 * Destroys a queue, freeing all it's allocated memory.
 */
void queue_destroy(Queue *q) {
    if (!queue_isEmpty(q)) {
        Item *i = q -> first;
        while (i != NULL) {
            /* first element of the queue is now the next element */
            q -> first = i -> next;
            free(i -> data);
            free(i);
            i = q -> first;
        }
    }
    free(q);
}

/*
 * queue_unitTest():
 * Unit test.
 */
void queue_unitTest() {
    puts("queue_unitTest()");
    Queue *queue = queue_new();
    queue_enqueue(queue, "teste1", sizeof("teste1"));
    queue_enqueue(queue, "teste2", sizeof("teste2"));
    queue_enqueue(queue, "teste3", sizeof("teste3"));
    printf("retorno = %s\n", queue_dequeue(queue));
    printf("retorno = %s\n", queue_dequeue(queue));
    printf("retorno = %s\n", queue_dequeue(queue));
    puts("");
    queue_destroy(queue);
}

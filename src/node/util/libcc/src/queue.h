/*******************************************************************************
 * Queue: FIFO collection of non-unique elements
 * Dependencies: stdio.h, stdlib.h, string.h
 * Compilation:
     $ make queue.o
     $ gcc file.c queue.o -o file
 * Include this header in your file
 ******************************************************************************/

#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct Queue Queue;

/*
 * Available operations
 */
Queue *queue_new();                                         // constructor
int queue_isEmpty(Queue *s);                                // is queue empty?
int queue_size(Queue *s);                                   // returns q size
int queue_enqueue(Queue *q, void *data, size_t dataSize);   // enqueues
void *queue_dequeue(Queue *s);                              // dequeues
void *queue_peek(Queue *s);                                 // returns head of q
void **queue_iterator(Queue *s);                            // iterator
void queue_unitTest();                                      // test
void queue_destroy(Queue *q);                               // frees memory

/* Let's avoid having to pass sizeof(x) as argument every time we add something.
 * Instead write queue_enqueue(q, data) and this macro will expand sizeof.
 */
#define queue_enqueue(q, data) queue_enqueue(q, data, sizeof(data))

#endif /* _QUEUE_H_ */

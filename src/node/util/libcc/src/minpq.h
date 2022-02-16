
/*******************************************************************************
 * MinPQ: heap-based minimum priority queue
 * Dependencies: stdio.h, stdlib.h, string.h
 * Compilation:
 $ make minpq.o
 $ gcc file.c minpq.o -o file
 * Include this header in your file
 ******************************************************************************/

#ifndef _MINPQ_H_
#define _MINPQ_H_

typedef struct MinPQ MinPQ;

/*
 * Available operations
 */
MinPQ *minpq_new();                                             // constructor
int minpq_isEmpty(MinPQ *mpq);                                  // is mpq empty?
int minpq_size(MinPQ *mpq);                                     // returns size
char *minpq_min(MinPQ *mpq);                                    // returns min
int minpq_insert(MinPQ *mpq, char *key);                        // inserts el
char *minpq_delMin(MinPQ *mpq);                                 // remove & ret
void minpq_destroy(MinPQ *mpq);                                 // frees memory
int minpq_maxkey(MinPQ *mpq);                                   // ret max_key


#endif /* _MINPQ_H_ */

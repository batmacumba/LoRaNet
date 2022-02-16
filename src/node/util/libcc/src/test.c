/*
 * Includes & Macros
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bag.h"
#include "stack.h"
#include "queue.h"
#include "vector.h"
#include "heap.h"

int main() {
    bag_unitTest();
    stack_unitTest();
    queue_unitTest();
    vector_unitTest();
    heap_unitTest();
    return 0;
}

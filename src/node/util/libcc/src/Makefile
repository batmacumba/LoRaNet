CC := gcc
CCFLAGS := -Ofast -msse2 -march=native -Wall -Wextra -Wpedantic

all: bag.o queue.o stack.o vector.o heap.o

test: bag.o queue.o stack.o vector.o heap.o
	$(CC) test.c *.o -o $@

bag.o: bag.c
	$(CC) $(CCFLAGS) -c $< -o $@
queue.o: queue.c
	$(CC) $(CCFLAGS) -c $< -o $@
stack.o: stack.c
	$(CC) $(CCFLAGS) -c $< -o $@
vector.o: vector.c
	$(CC) $(CCFLAGS) -c $< -o $@
heap.o: heap.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm *.o test

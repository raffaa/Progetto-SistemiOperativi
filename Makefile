CC=gcc
CCOPTS=--std=gnu99 -Wall -D_LIST_DEBUG_ 
AR=ar

OBJS=bit_map.o\
     buddy_allocator.o

HEADERS=bit_map.h buddy_allocator.h

LIBS=libbuddy.a

BINS=buddy_allocator_test_1 buddy_allocator_test_2

.phony: clean all


all:	$(LIBS) $(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

libbuddy.a: $(OBJS) 
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)

buddy_allocator_test_1: buddy_allocator_test_1.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm
	
buddy_allocator_test_2: buddy_allocator_test_2.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)

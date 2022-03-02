#include "buddy_allocator.h"
#include <stdio.h>

#define BUFFER_SIZE 128
#define BUDDY_LEVELS 3
#define MEMORY_SIZE 128
#define MIN_BUCKET_SIZE (BUFFER_SIZE>>(BUDDY_LEVELS))

uint8_t  buffer[BUFFER_SIZE]; 
char memory[MEMORY_SIZE];

BuddyAllocator alloc;
int main(int argc, char** argv) {
 
  // we initialize the allocator
  BuddyAllocator_init(&alloc, BUDDY_LEVELS,
                      buffer,
                      BUFFER_SIZE,
                      memory,
                      MIN_BUCKET_SIZE);
  printf("\n");
 
  printf("\n--------------- TEST 1/2 ---------------\n");
  printf("Empty bitmap:\n");
  BitMap_print(&alloc.bitmap);
  
  printf("\n---------------- MALLOC ----------------\n");
  void* p1=BuddyAllocator_malloc(&alloc, 50); // on level 1
  BitMap_print(&alloc.bitmap);
  printf("------------------------------------------\n");
  void* p2=BuddyAllocator_malloc(&alloc, 28); // on level 2
  BitMap_print(&alloc.bitmap);
  printf("------------------------------------------\n");
  void* p3=BuddyAllocator_malloc(&alloc, 11); // on level 3
  BitMap_print(&alloc.bitmap);
  printf("------------------------------------------\n");
  void* p4=BuddyAllocator_malloc(&alloc, 5); // on level 3
  BitMap_print(&alloc.bitmap);
  printf("------------------------------------------\n");
  printf("At this point the bitmap should be full!\n");
    
  printf("\n----------------- FREE -----------------\n");
  BuddyAllocator_free(&alloc, p1 );
  BitMap_print(&alloc.bitmap);
  printf("------------------------------------------\n");
  BuddyAllocator_free(&alloc, p2);
  BitMap_print(&alloc.bitmap);
  printf("------------------------------------------\n");
  BuddyAllocator_free(&alloc, p3);
  BitMap_print(&alloc.bitmap);
  printf("------------------------------------------\n");
  BuddyAllocator_free(&alloc, p4);
  BitMap_print(&alloc.bitmap);
  printf("------------------------------------------\n");
  printf("The bitmap should be empty again!\n");
  
  printf("\n----------------- END -----------------\n");
}

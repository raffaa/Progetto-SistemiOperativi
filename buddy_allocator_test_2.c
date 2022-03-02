#include "buddy_allocator.h"
#include <stdio.h>

#define BUFFER_SIZE 1024
#define BUDDY_LEVELS 5
#define MEMORY_SIZE 1024
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
 
  printf("\n--------------- TEST 2/2 ---------------\n");
 
  printf("Requesting a memory area bigger than memory\nsize should return an error\n\n");
  void* p1=BuddyAllocator_malloc(&alloc, 2000); 
  printf("------------------------------------------\n");
  
  printf("Requesting a memory area on level 0...\n");
  void* p2=BuddyAllocator_malloc(&alloc, 1000); 
  printf("\n");
  printf("...means there can't be any more requests...\n");
  void* p3=BuddyAllocator_malloc(&alloc, 20);
  printf("\n");
  printf("Releasing memory area...\n "); 
  BuddyAllocator_free(&alloc, p2);
  printf("\n");
  printf("...released.\n ");
  printf("------------------------------------------\n");
  
  printf("Allocating some blocks...\n ");
  printf("\n");
  void* p4=BuddyAllocator_malloc(&alloc, 58);
  printf("\n");
  void* p5=BuddyAllocator_malloc(&alloc, 110);
  printf("\n");
  void* p6=BuddyAllocator_malloc(&alloc, 22);
  printf("------------------------------------------\n");
  
  printf("Releasing some blocks...\n ");
  printf("\n");
  BuddyAllocator_free(&alloc, p5);
  printf("\n");
  BuddyAllocator_free(&alloc, p6);
  printf("\n");
  BuddyAllocator_free(&alloc, p4);
  printf("------------------------------------------\n");
  
  printf("Some exceptions...\n ");
  printf("\n");
  printf("Releasing null memory should return an error...\n ");
  BuddyAllocator_free(&alloc, p1);
  printf("\n");
  printf("Releasing memory twice should return an error...\n ");
  BuddyAllocator_free(&alloc, p5);
  
  printf("\n------------------ END -----------------\n");
}

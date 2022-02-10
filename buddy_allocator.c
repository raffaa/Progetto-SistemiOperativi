#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"

// these are trivial helpers to support you in case you want
// to do a bitmap implementation
int levelIdx(size_t idx){
  return (int)floor(log2(idx));
};

int buddyIdx(int idx){
  if (idx&0x1){
    return idx-1;
  }
  return idx+1;
}

int parentIdx(int idx){
  return idx/2;
}

int startIdx(int idx){
  return (idx-(1<<levelIdx(idx)));
}


// computes the size in bytes for the allocator
int BuddyAllocator_calcSize(int num_levels) {
  int buddies=1<<(num_levels+1); // maximum number of allocations, used to determine the max buddies
  int alloc_size=sizeof(int)*buddies+sizeof(int); //one more byte to store additional inf about the bitmap
  return alloc_size;
}


void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         char* memory, // allocator buffer
                         int mem_size,
                         int min_bucket_size,
                         int* bitmap, // bitmap buffer
                         int bitmap_size) {
  // initializing fiels of the struct
  alloc->num_levels=num_levels;
  alloc->memory=memory;
  alloc->mem_size=mem_size;
  alloc->min_bucket_size=min_bucket_size;
  
  assert ("Number of levels bigger than maximum number of levels allowed!" && num_levels<MAX_LEVELS);

  int buddies=1<<(num_levels+1); // maximum number of allocations, used to size the bitmap
  
  if (bitmap_size >= BitMap_getBytes(buddies)) 
	printf("Not enough memory fo the bitmap!");

  printf("BUDDY INITIALIZING\n");
  printf("\tlevels: %d", num_levels);
  printf("\tmax list entries %d bytes\n", buddies);
  printf("\tbucket size:%d\n", min_bucket_size);
  printf("\tmanaged memory %d bytes\n", (1<<num_levels)*min_bucket_size);
  
  alloc->bitmap_size=bitmap_size;
  BitMap_init(alloc->bitmap, bitmap_size, alloc->memory);
}

//reverse loop to find smallest buddy
int BuddyAllocator_minBuddy(BuddyAllocator* alloc, int num_levels){
  for (int i=(1<<num_levels+1); i>=0; --i) {
	if (BitMap_getBit(alloc->bitmap, i)
	  return i;  
  }
  printf("ERROR! No more buddies available.\n");
  return 0;
}

int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){
  if (level<0)
    return 0;
  assert(level <= alloc->num_levels);

  int idx=BuddyAllocator_minBuddy(alloc);
  
  if(idx>0) {
	//means some memory has been requested, therefore not every node is free anymore   
	
	//find parent of node idx   
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
  } else {
	printf("ERROR! No more buddies available.\n");
    return 0;
  }
  return idx;
}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, int idx){

	printf("Free memory at %d\n", idx);
	assert("ERROR! Double free." && BitMap_bit(alloc->bitmap, idx));
/*  BuddyListItem* parent_ptr=item->parent_ptr;
  BuddyListItem *buddy_ptr=item->buddy_ptr;
  
  // buddy back in the free list of its level
  List_pushFront(&alloc->free[item->level],(ListItem*)item);

  // if on top of the chain, do nothing
  if (! parent_ptr)
    return;
  
  // if the buddy of this item is not free, we do nothing
  if (buddy_ptr->list.prev==0 && buddy_ptr->list.next==0) 
    return;
*/  
  //join
  //1. we destroy the two buddies in the free list;
  printf("merge %d\n", levelIdx(idx));
  // set children status
  BitMap_setBit(alloc->bitmap, idx, 0);
  
  //2. we release the parent
  BuddyAllocator_releaseBuddy(alloc, parentIdx(idx));

}

//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
  size += sizeof(int); // additional memory to store bitmap index
  assert("Not enough memory available!" && (alloc->mem_size) < size);
  
  // we determine the level of the page
  int level=floor(log2(mem_size/(size+8))); // +8 of bookkeping

  // if the level is too small, we pad it to max
  if (level>alloc->num_levels)
    level=alloc->num_levels;

  printf("requested: %d bytes, level %d \n",
         size, level);
  
  // we get a buddy of that size;
  int buddyIdx=BuddyAllocator_getBuddy(alloc, level);
  if ((alloc->bitmap)[buddyIdx]==1) // memory already in use
    return NULL;

  // we write in the memory region managed the buddy address
  char* target=alloc->memory;
  ((int*)target)[0]=buddyIdx; // bookkeeping
   
  return (void*)(target+sizeof(int));
}
//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
  printf("freeing %p", mem);
  // we retrieve the buddy from the system
  mem=mem-8;
  int* mem_ptr=(int*) mem;
  int buddyIdx=*mem_ptr;
  // sanity check;
  //assert(alloc->memory==mem_ptr);
  BuddyAllocator_releaseBuddy(alloc, buddyIdx);
  
}

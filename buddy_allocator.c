#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"

// these are trivial helpers to support you in case you want
// to do a bitmap implementation

// level of node idx
int levelIdx(size_t idx){
  return (int)floor(log2(idx));
};

// idx of 1st node of a level i
int firstIdx(int i){
  return 1<<i;  /*{2^i}*/
}

// offset of node idx in his level
int startIdx(int idx){
  return idx-firstIdx(levelIdx(idx)); //(idx-(1<<levelIdx(idx)));
}

// index of the buddy of node idx
int buddyIdx(int idx){
  if (idx&0x1){
    return idx-1;
  }
  return idx+1;
}

// parent of the node idx
int parentIdx(int idx){
  return idx/2;
}

// initializes the buddy allocator, and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,
                         char* memory, // allocator buffer
                         int mem_size,
                         int min_bucket_size,
                         int* bitmap, // bitmap buffer
                         int bitmap_size) {
  
  assert ("Minimum bucket size too small!" && min_bucket_size<8);
  assert ("Number of levels bigger than maximum number of levels allowed!" && num_levels<MAX_LEVELS);
  
  int buddies=1<<(num_levels+1); // maximum number of allocations, used to size the bitmap
  assert ("Not enough memory fo the bitmap!" && bitmap_size >= BitMap_getBytes(buddies));

  // initializing fiels of the struct
  alloc->num_levels=num_levels;
  alloc->memory=memory;
  alloc->mem_size=mem_size;
  alloc->min_bucket_size=min_bucket_size;

  printf("BUDDY INITIALIZING\n");
  printf("\tlevels: %d", num_levels);
  printf("\tmax list entries %d bytes\n", buddies);
  printf("\tbucket size:%d\n", min_bucket_size);
  printf("\tmanaged memory %d bytes\n", mem_size); //(1<<num_levels)*min_bucket_size
  
  alloc->bitmap_size=bitmap_size;
  BitMap_init(alloc->bitmap, bitmap_size, memory);
  printf("Bitmap initialized successfully!");
}

//reverse loop to find smallest buddy
int BuddyAllocator_minBuddy(BuddyAllocator* alloc, int num_levels){
  int idx=-1;
  for (int i=(1<<num_levels+1); i>=0; --i) {
	if (!BitMap_getBit(alloc->bitmap, i)) // if not taken
	  idx = i;  
  }
  printf("ERROR! No more buddies available.\n");
  return idx;
}

int BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level){
  assert(level < 0);
  assert(level <= alloc->num_levels);

  int idx=BuddyAllocator_minBuddy(alloc, level);   
  if (idx<0) {
    printf("ERROR! No more buddies available.\n");
    return idx;
  }
  
  BitMap_setBit(alloc->bitmap, idx, 1);
  BitMap_setBit(alloc->bitmap, buddyIdx(idx), 1);
  BuddyAllocator_getBuddy(alloc, level-1);
  
  return idx;
}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, int idx){

  printf("Free memory at %d\n", idx);
  assert("ERROR! Double free." && BitMap_bit(alloc->bitmap, idx));

  //join
  //1. we destroy the two buddies in the free list;
  printf("merge %d\n", levelIdx(idx));
  // set children status
  if (idx < alloc->bitmap->num_bits) {
    BitMap_setBit(alloc->bitmap, idx, 0);
    BitMap_setBit(alloc->bitmap, buddyIdx(idx), 0);
  } 
  //2. we release the parent
  BuddyAllocator_releaseBuddy(alloc, parentIdx(idx));

}

//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
  // the size+8 is rounded up to the size of the smallest partition capable to contain it
  // calculate max mema
  int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size;
 
  assert("Not enough memory available!" && (alloc->mem_size) < mem_size);
  
  // calculate level for page
  int level=floor(log2(mem_size/(size+8))); // +8 of bookkeping
  // if the level is too small, we pad it to max
  if (level>alloc->num_levels)
    level=alloc->num_levels;

  printf("requested: %d bytes, level %d \n", size, level);
  
  // we get a buddy of that size;
  int buddyIdx=BuddyAllocator_getBuddy(alloc, level);
  if ((alloc->bitmap)[buddyIdx]==1) // memory already in use
    return NULL;

  // we write in the memory region managed the buddy address
  uint8_t* target=alloc->memory+startIdx(buddyIdx);
  ((int*)target)[0]=buddyIdx; // bookkeeping
  
  printf("Bitmap allocated successfully!");
   
  return (void*)(target + sizeof(int)); // to hide bookkeeping info
}
//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
  printf("Freeing %p\n", mem);
  
  assert("Cannot free NULL memory area" && mem);
  
  // we retrieve the buddy from the system
  mem=mem-8;
  int* mem_ptr=(int*) mem;
  int to_free=*mem_ptr;

  BuddyAllocator_releaseBuddy(alloc, to_free);
  
  printf("Bitmap freed successfully!");
}

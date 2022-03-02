#pragma once
#include <stdint.h>

// simple bit array
typedef struct  {
  uint8_t *buffer;  // externally allocated buffer
  int buffer_size;
  int num_bits; 
} BitMap;

// N.B. levels start from 0
// level of node idx
int levelIdx(int idx);

// idx of 1st node of a level i
int firstIdx(int i);

// offset of node idx in his level
// N.B. starts from 0
int startIdx(int idx);

// index of the buddy of node idx
int buddyIdx(int idx);

// parent of the node idx
int parentIdx(int idx);

// returns the number of bytes to store bits booleans
int BitMap_getBytes(int bits);

// initializes a bitmap on an external array
void BitMap_init(BitMap* bit_map, int num_bits, uint8_t* buffer);

// sets the bit bit_num in the bitmap
// status= 0 or 1
void BitMap_setBit(BitMap* bit_map, int bit_num, int status);

// inspects the status of the bit bit_num
int BitMap_bit(const BitMap* bit_map, int bit_num);

/////////////// aux functions /////////////// 

// prints bitmap structure
// 1=available, 0=unavailable/released
void BitMap_print(BitMap* bitmap);

// recursively sets the "descendants" bits of bit_num in the bitmap
void BitMap_setBit_children(BitMap* bitmap, int bit_num, int status);

// recursively sets the "ancestors" bits of bit_num in the bitmap
void BitMap_setBit_parents(BitMap* bitmap, int bit_num, int status);

// recursive merge
void Bitmap_merge(BitMap *bitmap, int idx);

// returns 1 if all children of idx are free, 0 otherwise
int BitMap_check_children(BitMap* bit_map, int idx);

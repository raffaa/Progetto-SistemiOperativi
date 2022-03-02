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

// prints bitmap structure
// 0: at list one of the children is free  
// 1: is taken (or both children are taken)
void BitMap_print(BitMap* bitmap);

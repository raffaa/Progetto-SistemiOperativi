# Buddy Memory Allocator with Bitmap Management

This project implements a buddy memory allocation system using bitmap-based tracking. It was developed as part of the Operating Systems course at "La Sapienza" University of Rome.

Please refer to [Buddy Allocator](https://en.wikipedia.org/wiki/Buddy_memory_allocation) for the theoretical background.

## Features
- Buddy system for efficient dynamic memory allocation
- Memory block splitting and merging
- Bitmap structure to manage free and allocated blocks

## Files Overview

### 1. **bit_map.c / bit_map.h**
These files contain the implementation of the bitmap structure used for tracking the status of memory blocks (free or allocated). The bitmap provides a simple and efficient way to manage the memory pool, keeping track of which memory regions are available for allocation.

### 2. **buddy_allocator.c / buddy_allocator.h**
These files contain the core implementation of the buddy memory allocation algorithm. The buddy allocator is responsible for splitting and merging memory blocks based on the request size, using the buddy system to ensure efficient memory usage.

### 3. **makefile**
The `Makefile` automates the build process, compiling the C source files into executables. It defines the dependencies between source files and ensures that the project is built correctly with all the necessary flags for compilation.

### 4. **buddy_allocator_test_1.c / buddy_allocator_test_2.c**
These test files contain example programs to test the functionality of the buddy memory allocator. The tests include various scenarios for memory allocation and deallocation (```buddy_allocator_test_1.c```), ensuring that the allocator behaves correctly and efficiently under different conditions (```buddy_allocator_test_2.c```).

## How to Compile and Run

To compile the project, run the following command:

```bash
make
```

After compilation, you can run the tests to verify the functionality of the buddy memory allocator.

```bash
./buddy_allocator_test_1
./buddy_allocator_test_2
```

/*
MIT License

Copyright (c) 2018 Danis Ozdemir

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdint.h>
#include <stdbool.h>

// Ordinary mempool declarations
typedef struct mempool mempool;

mempool *mempool_create(uint32_t elem_count, uint32_t elem_size,
                        bool fallback_to_dynamic_memory);

void _mempool_destroy(mempool *mp);

#define mempool_destroy(mp) \
  do {                      \
    _mempool_destroy(mp);   \
    mp = NULL;              \
  } while (0)

void *mempool_alloc_entry(mempool *mp);

void *mempool_calloc_entry(mempool *mp);

void _mempool_free_entry(void *entry);

#define mempool_free_entry(entry) \
  do {                            \
    _mempool_free_entry(entry);   \
    entry = NULL;                 \
  } while (0)

uint32_t mempool_total_capacity(mempool *mp);

uint32_t mempool_used_count(mempool *mp);

uint32_t mempool_dynamic_allocs_count(mempool *mp);

// Ranged mempool declarations
typedef struct r_mempool r_mempool;

typedef enum r_memory_fallback_policy_t {
  fallback_disabled = 0,
  fallback_at_first_exhaustion,
  fallback_at_last_exhaustion,
  // This one should always remain at the end
  __fallback_end_place_holder
} r_memory_fallback_policy_t;

// The following function will create an 'array' of memory pools
// that covers the buffer sizes starting from the smallest_size
// up to the largest_size. These sizes will be 'ceiled' to the
// closest power of two. The number of elements in each memory
// pool will be the half of its predecessor, and double of its
// successor. The number of the 'smallest_sized' elements will
// be number_of_smallest_size_elems (Again, 'ceiled' to the
// closest power of two).
r_mempool *r_mempool_create(uint32_t smallest_size, uint32_t largest_size,
                            uint32_t number_of_smallest_size_elems,
                            r_memory_fallback_policy_t fb_policy);

void _r_mempool_destroy(r_mempool *rmp);

#define r_mempool_destroy(rmp) \
  do {                         \
    _r_mempool_destroy(rmp);   \
    rmp = NULL;                \
  } while (0)

uint32_t r_mempool_used_count(r_mempool *rmp, uint32_t size);

uint32_t r_mempool_total_capacity(r_mempool *rmp, uint32_t size);

uint32_t r_mempool_dynamic_allocs_count(r_mempool *rmp, uint32_t size);

void *r_mempool_alloc_entry(r_mempool *rmp, uint32_t size);

void *r_mempool_calloc_entry(r_mempool *rmp, uint32_t size);

void *r_mempool_realloc_entry(r_mempool *rmp, void *addr, uint32_t size);

#define r_mempool_free_entry(entry) mempool_free_entry(entry)

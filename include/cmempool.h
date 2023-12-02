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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Ordinary mempool declarations
typedef struct mempool mempool;

// The following struct is only there to avoid using 'packed' structs
// and having a fixed offset for the '_final' field. It should never
// be used for any other purposes.
typedef struct __dummy_struct_for_offset_dont_use {
  uint32_t _0_;
  mempool *_1_;
  void *_final_;
} __dummy_struct_for_offset_dont_use;

mempool *mempool_create(uint32_t elem_count, uint32_t elem_size,
                        bool fallback_to_dynamic_memory,
                        bool will_be_accessed_by_only_one_thread);

#define DECLARE_STATIC_MEMPOOL_BUFFER(name, elem_count, elem_size) \
  static uint8_t                                                   \
      name[elem_count *                                            \
           (elem_size +                                            \
            offsetof(__dummy_struct_for_offset_dont_use, _final_))] = {0}

mempool *mempool_create_from_preallocated_buffer(
    void *buffer, uint32_t buf_size, uint32_t elem_size,
    bool fallback_to_dynamic_memory, bool will_be_accessed_by_only_one_thread);

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
// The ranged mempools are a quick alternative to dynamic memory
// allocation in which the memory is preallocated and served in
// a similar fashion to the heap.
// Here, unlike the 'normal' memory pools, the element sizes and
// the number of elements should all be powers of two. Just to
// clarify any of these input values would be used as "2^value".
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
r_mempool *r_mempool_create(uint8_t smallest_size_power_of_two,
                            uint8_t largest_size_power_of_two,
                            uint8_t number_of_smallest_size_elems_power_of_two,
                            r_memory_fallback_policy_t fb_policy,
                            bool will_be_accessed_by_only_one_thread);

// The macro CALCULATE_STATIC_RMEMPOOL_BUFFER_SIZE calculates
// the required size for a rmempool. It is not meant to be used
// directly, it is mostly there as a helper to the following macro
// DECLARE_STATIC_RMEMPOOL_BUFFER.
#define CALCULATE_STATIC_RMEMPOOL_BUFFER_SIZE(SS, LS, SC)                   \
  ((LS - SS + 1) * (1 << SC) * (1 << SS) +                                  \
   (2 * (1 << SC) * offsetof(__dummy_struct_for_offset_dont_use, _final_) * \
    ((1 << (LS - SS + 1)) - 1)) /                                           \
       (1 << (LS - SS + 1)))

// The macro DECLARE_STATIC_RMEMPOOL_BUFFER declares a static
// buffer using the given name and size parameters. This buffer
// then needs to be passed into the function
// r_mempool_create_from_preallocated_buffer to be 'organized'
// as a memory pool.
#define DECLARE_STATIC_RMEMPOOL_BUFFER(                          \
    name, smallest_size_power_of_two, largest_size_power_of_two, \
    number_of_smallest_size_elems_power_of_two)                  \
  static uint8_t name[CALCULATE_STATIC_RMEMPOOL_BUFFER_SIZE(     \
      smallest_size_power_of_two, largest_size_power_of_two,     \
      number_of_smallest_size_elems_power_of_two)] = {0}

r_mempool *r_mempool_create_from_preallocated_buffer(
    void *buffer, uint32_t buf_size, uint8_t smallest_size_power_of_two,
    uint8_t largest_size_power_of_two,
    uint8_t number_of_smallest_size_elems_power_of_two,
    r_memory_fallback_policy_t fb_policy,
    bool will_be_accessed_by_only_one_thread);

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

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

#include <cmempool.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

typedef uintptr_t *addr_t;

const char *_mempool_mark = "mempool";

#define ENTRY_TO_HEADER(entry) \
  (entry_header *)((uintptr_t)entry - offsetof(entry_header, next))

#define EXT_SIZE_TO_USER_SIZE(ext_elem_size) \
  (ext_elem_size - offsetof(entry_header, next))

#define USER_SIZE_TO_EXT_SIZE(elem_size) \
  (elem_size + offsetof(entry_header, next))

struct mempool {
  const char *mempool_mark;  // This field is used for sanity checks
  uint32_t ext_elem_size;
  uint32_t total_elem_count;
  bool fallback_to_dynamic_memory;
  uint32_t active_dynamic_memory_buffer_count;
  uintptr_t lower_addr_limit;
  uintptr_t upper_addr_limit;
  uint32_t free_elem_count;
  void *objects;
  void *free_inst;
  pthread_rwlock_t lock;
};

const uint32_t elem_is_free = 0xdeadbeef;
const uint32_t elem_is_taken = 0xfeedcafe;
const uint32_t elem_is_not_a_pool_member = 0xfadeface;

typedef struct entry_header {
  uint32_t elem_status;
  mempool *pool_ptr;
  uint32_t ext_elem_size;
  // The following field should always be the last field.
  addr_t next;
} entry_header;

void _mempool_destroy(mempool *mp) {
  if (mp) {
    if (mp->objects) free(mp->objects);
    pthread_rwlock_destroy(&mp->lock);
    free(mp);
  }
}

void mempool_init_internal_scalars(mempool *mp, uint32_t elem_count,
                                   uint32_t ext_elem_size,
                                   bool fallback_to_dynamic_memory) {
  for (uint32_t i = 0; i < elem_count; ++i) {
    entry_header *header =
        (entry_header *)((uintptr_t)mp->objects + i * ext_elem_size);
    header->elem_status = elem_is_free;
    header->pool_ptr = mp;
    header->ext_elem_size = ext_elem_size;

    if (i == (elem_count - 1)) {
      header->next = NULL;
    } else {
      header->next = (addr_t)((uintptr_t)mp->objects + (i + 1) * ext_elem_size);
    }
  }

  mp->free_inst = mp->objects;
  mp->mempool_mark = _mempool_mark;
  mp->ext_elem_size = ext_elem_size;
  mp->total_elem_count = elem_count;
  mp->fallback_to_dynamic_memory = fallback_to_dynamic_memory;
  mp->active_dynamic_memory_buffer_count = 0;
  mp->lower_addr_limit = (uintptr_t)mp->objects;
  mp->upper_addr_limit = (uintptr_t)mp->objects + ext_elem_size * elem_count;
  mp->free_elem_count = elem_count;
}

mempool *mempool_create(uint32_t elem_count, uint32_t elem_size,
                        bool fallback_to_dynamic_memory) {
  if (elem_count == 0 || elem_size == 0) {
    return NULL;
  } else if (elem_size < sizeof(addr_t)) {
    elem_size = sizeof(addr_t);
  }

  mempool *mp = (mempool *)calloc(1, sizeof(mempool));
  if (!mp) {
    return NULL;
  }

  uint32_t ext_elem_size = USER_SIZE_TO_EXT_SIZE(elem_size);
  mp->objects = calloc(elem_count, ext_elem_size);
  if (!mp->objects) {
    mempool_destroy(mp);
    return NULL;
  }

  if (pthread_rwlock_init(&mp->lock, NULL) != 0) {
    mempool_destroy(mp);
    return NULL;
  }

  mempool_init_internal_scalars(mp, elem_count, ext_elem_size,
                                fallback_to_dynamic_memory);

  return mp;
}

void *mempool_alloc_entry(mempool *mp) {
  if (!mp) {
    assert(false);
  }

  void *result = NULL;

  pthread_rwlock_wrlock(&mp->lock);

  if (mp->free_inst) {
    entry_header *header = (entry_header *)mp->free_inst;

    if (header->elem_status != elem_is_free || header->pool_ptr != mp) {
      // We have a corruption!
      pthread_rwlock_unlock(&mp->lock);
      assert(false);
    }

    mp->free_inst = header->next;
    header->elem_status = elem_is_taken;
    result = (void *)&header->next;
    --mp->free_elem_count;
  } else if (mp->fallback_to_dynamic_memory) {
    // Seems like we exhausted our buffers and
    // we are asked to fallback to the dynamic
    // memory allocation mechanisms.
    void *new_buffer = malloc(mp->ext_elem_size);
    if (new_buffer) {
      entry_header *header = (entry_header *)new_buffer;
      header->elem_status = elem_is_not_a_pool_member;
      header->pool_ptr = mp;
      header->ext_elem_size = mp->ext_elem_size;
      result = (void *)&header->next;
      ++mp->active_dynamic_memory_buffer_count;
    }
  }

  pthread_rwlock_unlock(&mp->lock);

  return result;
}

void *mempool_calloc_entry(mempool *mp) {
  void *result = mempool_alloc_entry(mp);

  if (result) {
    memset(result, 0, EXT_SIZE_TO_USER_SIZE(mp->ext_elem_size));
  }

  return result;
}

static inline bool valid_mempool_addr(mempool *mp, uintptr_t c_entry) {
  return (c_entry >= mp->lower_addr_limit) &&
         (c_entry < mp->upper_addr_limit) &&
         (c_entry - mp->lower_addr_limit) % mp->ext_elem_size == 0;
}

void __mempool_free_entry(mempool *mp, entry_header *header) {
  if (!mp) {
    assert(false);
  }

  uintptr_t c_header = (uintptr_t)header;

  pthread_rwlock_wrlock(&mp->lock);

  if (header->elem_status == elem_is_not_a_pool_member) {
    // We allocated this buffer when we had exhausted
    // our own buffers.
    if (mp->active_dynamic_memory_buffer_count == 0) {
      // Something is not right, most probably a double free
      pthread_rwlock_unlock(&mp->lock);
      assert(false);
    }
    --mp->active_dynamic_memory_buffer_count;
    free(header);
    pthread_rwlock_unlock(&mp->lock);
    return;
  }

  if (valid_mempool_addr(mp, c_header)) {
    if (mp->ext_elem_size != header->ext_elem_size) {
      // The size got overwritten.
      assert(false);
    }

    if (header->elem_status != elem_is_taken) {
      // This block seems tampered

      addr_t addr = header->next;
      if (valid_mempool_addr(mp, (uintptr_t)(*addr))) {
        // Was this address returned to the pool before?
        if (header->elem_status == elem_is_free) {
          // Double free!
          pthread_rwlock_unlock(&mp->lock);
          assert(false);
        }
      }

      // Somehow the entry got overwritten.
      pthread_rwlock_unlock(&mp->lock);
      assert(false);
    }

    header->elem_status = elem_is_free;
    header->next = (addr_t)mp->free_inst;
    mp->free_inst = header;
    ++mp->free_elem_count;
  } else {
    pthread_rwlock_unlock(&mp->lock);
    assert(false);
  }

  pthread_rwlock_unlock(&mp->lock);
}

void _mempool_free_entry(void *entry) {
  if (!entry) {
    // Let's resemble the dynamic memory allocation approach here.
    // Releasing a NULL pointer is acceptable.
    return;
  }

  entry_header *header = ENTRY_TO_HEADER(entry);

  // Let's check the invariant parts.
  if (!header) {
    assert(false);
  }

  if (!header->pool_ptr) {
    assert(false);
  }

  if (header->pool_ptr->mempool_mark != _mempool_mark) {
    assert(false);
  }

  // Passed the initial checks, no corruption so far.
  __mempool_free_entry(header->pool_ptr, header);
}

uint32_t mempool_total_capacity(mempool *mp) {
  if (!mp) {
    assert(false);
  }

  uint32_t result = 0;

  pthread_rwlock_rdlock(&mp->lock);
  result = mp->total_elem_count;
  pthread_rwlock_unlock(&mp->lock);

  return result;
}

uint32_t mempool_used_count(mempool *mp) {
  if (!mp) {
    assert(false);
  }

  uint32_t result = 0;

  pthread_rwlock_rdlock(&mp->lock);
  result = mp->total_elem_count - mp->free_elem_count;
  pthread_rwlock_unlock(&mp->lock);

  return result;
}

uint32_t mempool_dynamic_allocs_count(mempool *mp) {
  if (!mp) {
    assert(false);
  }

  uint32_t result = 0;

  pthread_rwlock_rdlock(&mp->lock);
  result = mp->active_dynamic_memory_buffer_count;
  pthread_rwlock_unlock(&mp->lock);

  return result;
}

// Ranged memory pool implementation starts
const uint32_t min_allowed_smallest_size = 16;
const uint32_t max_allowed_largest_size = 131072;

struct r_mempool {
  mempool **mem_pools;  // The real memory pools
  mempool pseudo_pool;
  r_memory_fallback_policy_t fb_policy;
  uint32_t number_of_mempools;
  uint32_t *reverse_size_lookup_array;
  uint32_t reverse_size_lookup_array_length;
  uint32_t smallest_size;
  uint32_t largest_size;
  uint32_t smallest_elem_count;
};

void _r_mempool_destroy(r_mempool *rmp) {
  if (rmp) {
    if (rmp->mem_pools) {
      for (uint32_t i = 0; i < rmp->number_of_mempools; ++i) {
        if (rmp->mem_pools[i]) {
          mempool_destroy(rmp->mem_pools[i]);
        }
      }
      free(rmp->mem_pools);
    }
    if (rmp->reverse_size_lookup_array) {
      free(rmp->reverse_size_lookup_array);
    }

    if (rmp->fb_policy == fallback_at_last_exhaustion) {
      pthread_rwlock_destroy(&rmp->pseudo_pool.lock);
    }

    free(rmp);
  }
}

bool adjust_and_assess_r_mempool_create_inputs(
    r_mempool *rmp, uint32_t *smallest_size, uint32_t *largest_size,
    uint32_t *smallest_elem_count, r_memory_fallback_policy_t fb_policy) {
  if (*smallest_size == 0 || *largest_size == 0 || *smallest_elem_count == 0) {
    return false;
  }

  *smallest_size = powl(2, ceill(log2l(*smallest_size)));
  *largest_size = powl(2, ceill(log2l(*largest_size)));
  *smallest_elem_count = powl(2, ceill(log2l(*smallest_elem_count)));

  if (*largest_size <= *smallest_size ||
      *largest_size > max_allowed_largest_size ||
      *smallest_size < min_allowed_smallest_size) {
    return false;
  }

  if (fb_policy < 0 || fb_policy >= __fallback_end_place_holder) {
    return false;
  }

  rmp->smallest_size = *smallest_size;
  rmp->largest_size = *largest_size;
  rmp->smallest_elem_count = *smallest_elem_count;
  rmp->number_of_mempools = log2l(*largest_size) - log2l(*smallest_size) + 1;
  rmp->reverse_size_lookup_array_length = (*largest_size) / (*smallest_size);

  return true;
}

bool init_r_mempool_pseudo_pool(r_mempool *rmp) {
  memset(&rmp->pseudo_pool, 0, sizeof(mempool));
  if (rmp->fb_policy == fallback_at_last_exhaustion) {
    if (pthread_rwlock_init(&rmp->pseudo_pool.lock, NULL) != 0) {
      return false;
    }
    rmp->pseudo_pool.fallback_to_dynamic_memory = true;
    rmp->pseudo_pool.mempool_mark = _mempool_mark;
  }

  return true;
}

bool init_r_mempool_internal_pools(r_mempool *rmp) {
  if (!init_r_mempool_pseudo_pool(rmp)) {
    return false;
  }

  rmp->mem_pools =
      (mempool **)malloc(rmp->number_of_mempools * sizeof(mempool));
  if (!rmp->mem_pools) {
    // The cleanup will be performed by the caller.
    return false;
  }
  memset(rmp->mem_pools, 0, rmp->number_of_mempools * sizeof(mempool));

  uint32_t first_size = rmp->smallest_size;
  uint32_t last_size = rmp->largest_size;
  uint32_t first_count = rmp->smallest_elem_count;

  for (uint32_t esize = first_size, ecount = first_count, index = 0;
       esize <= last_size; esize *= 2, ecount /= 2, ++index) {
    rmp->mem_pools[index] = mempool_create(
        ecount, esize, rmp->fb_policy == fallback_at_first_exhaustion);
    if (!rmp->mem_pools[index]) {
      // The cleanup will be performed by the caller.
      return false;
    }
  }

  return true;
}

bool init_r_mempool_reverse_size_lookup_array(r_mempool *rmp) {
  rmp->reverse_size_lookup_array = (uint32_t *)malloc(
      rmp->largest_size / rmp->smallest_size * sizeof(uint32_t));
  if (!rmp->reverse_size_lookup_array) {
    // The cleanup will be performed by the caller.
    return false;
  }

  uint32_t threshold_plus_one = 1;
  uint32_t target_index = 0;
  for (uint32_t i = 0; i < rmp->reverse_size_lookup_array_length; ++i) {
    rmp->reverse_size_lookup_array[i] = target_index;
    if (i == (threshold_plus_one - 1)) {
      ++target_index;
      threshold_plus_one *= 2;
    }
  }

  return true;
}

r_mempool *r_mempool_create(uint32_t smallest_size, uint32_t largest_size,
                            uint32_t smallest_elem_count,
                            r_memory_fallback_policy_t fb_policy) {
  r_mempool *rmp = (r_mempool *)malloc(sizeof(r_mempool));
  if (!rmp) {
    return NULL;
  }
  memset(rmp, 0, sizeof(r_mempool));

  if (!adjust_and_assess_r_mempool_create_inputs(
          rmp, &smallest_size, &largest_size, &smallest_elem_count,
          fb_policy)) {
    r_mempool_destroy(rmp);
    return NULL;
  }
  rmp->fb_policy = fb_policy;

  if (!init_r_mempool_internal_pools(rmp)) {
    r_mempool_destroy(rmp);
    return NULL;
  }

  if (!init_r_mempool_reverse_size_lookup_array(rmp)) {
    r_mempool_destroy(rmp);
    return NULL;
  }

  return rmp;
}

void *mempool_pseudo_alloc_entry(mempool *mp, uint32_t elem_size) {
  void *result = NULL;

  if (elem_size < sizeof(addr_t)) {
    elem_size = sizeof(addr_t);
  }

  uint32_t ext_elem_size = USER_SIZE_TO_EXT_SIZE(elem_size);

  pthread_rwlock_wrlock(&mp->lock);
  void *new_buffer = malloc(ext_elem_size);
  if (new_buffer) {
    entry_header *header = (entry_header *)new_buffer;
    header->elem_status = elem_is_not_a_pool_member;
    header->pool_ptr = mp;
    header->ext_elem_size = ext_elem_size;
    result = (void *)&header->next;
    ++mp->active_dynamic_memory_buffer_count;
  }
  pthread_rwlock_unlock(&mp->lock);

  return result;
}

void *r_mempool_alloc_entry(r_mempool *rmp, uint32_t size) {
  if (!rmp || size == 0 || size > rmp->largest_size) {
    return NULL;
  }

  uint32_t index = (size - 1) / rmp->smallest_size;

  void *result = NULL;

  for (; index <= rmp->number_of_mempools; ++index) {
    result = mempool_alloc_entry(
        rmp->mem_pools[rmp->reverse_size_lookup_array[index]]);
    if (result) {
      break;
    }
  }

  if (!result && rmp->fb_policy == fallback_at_last_exhaustion) {
    result = mempool_pseudo_alloc_entry(&rmp->pseudo_pool, size);
  }

  return result;
}

void *r_mempool_calloc_entry(r_mempool *rmp, uint32_t size) {
  void *result = r_mempool_alloc_entry(rmp, size);

  if (result) {
    memset(result, 0, size);
  }

  return result;
}

void *r_mempool_realloc_entry(r_mempool *rmp, void *addr, uint32_t size) {
  if (!rmp || size == 0 || size > rmp->largest_size) {
    return NULL;
  }

  uint32_t min_user_size = 0;

  if (addr) {
    entry_header *header = ENTRY_TO_HEADER(addr);

    uint32_t index = (size - 1) / rmp->smallest_size;
    uint32_t new_ext_size =
        rmp->mem_pools[rmp->reverse_size_lookup_array[index]]->ext_elem_size;

    if (new_ext_size == header->ext_elem_size) {
      // The requested size matches the current
      // size, return the original pointer.
      return addr;
    }

    min_user_size = EXT_SIZE_TO_USER_SIZE(header->ext_elem_size);
    if (EXT_SIZE_TO_USER_SIZE(new_ext_size) < min_user_size) {
      min_user_size = EXT_SIZE_TO_USER_SIZE(new_ext_size);
    }
  }

  void *new_entry = r_mempool_alloc_entry(rmp, size);
  if (new_entry && addr) {
    memcpy(new_entry, addr, min_user_size);
    mempool_free_entry(addr);
  }

  return new_entry;
}

uint32_t r_mempool_used_count(r_mempool *rmp, uint32_t size) {
  if (!rmp || size == 0 || size > rmp->largest_size) {
    return 0;
  }

  uint32_t index = (size - 1) / rmp->smallest_size;

  return mempool_used_count(
      rmp->mem_pools[rmp->reverse_size_lookup_array[index]]);
}

uint32_t r_mempool_total_capacity(r_mempool *rmp, uint32_t size) {
  if (!rmp || size == 0 || size > rmp->largest_size) {
    return 0;
  }

  uint32_t index = (size - 1) / rmp->smallest_size;

  return mempool_total_capacity(
      rmp->mem_pools[rmp->reverse_size_lookup_array[index]]);
}

uint32_t r_mempool_dynamic_allocs_count(r_mempool *rmp, uint32_t size) {
  if (!rmp || size == 0 || size > rmp->largest_size) {
    return 0;
  }

  if (rmp->fb_policy == fallback_disabled) {
    return 0;
  }

  if (rmp->fb_policy == fallback_at_first_exhaustion) {
    uint32_t index = (size - 1) / rmp->smallest_size;

    return mempool_dynamic_allocs_count(
        rmp->mem_pools[rmp->reverse_size_lookup_array[index]]);
  }

  return mempool_dynamic_allocs_count(&rmp->pseudo_pool);
}

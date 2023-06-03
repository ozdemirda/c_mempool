#include <cmempool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <tau/tau.h>
TAU_MAIN()  // sets up Tau (+ main function)

// C_MEMPOOL TESTS

extern uint32_t find_nearest_gte_power_of_two(uint32_t input);

TEST(cmempools, find_nearest_gte_power_of_two) {
  // Hand-crafted the following input and expected output arrays, as we don't
  // want to depend on a math lib to do the testing.

  uint32_t test_inputs[] = {
      0,          1,          2,          3,          4,          5,
      7,          8,          9,          15,         16,         17,
      31,         32,         33,         63,         64,         65,
      127,        128,        129,        255,        256,        257,
      511,        512,        513,        1023,       1024,       1025,
      2047,       2048,       2049,       4095,       4096,       4097,
      8191,       8192,       8193,       16383,      16384,      16385,
      32767,      32768,      32769,      65535,      65536,      65537,
      131071,     131072,     131073,     262143,     262144,     262145,
      524287,     524288,     524289,     1048575,    1048576,    1048577,
      2097151,    2097152,    2097153,    4194303,    4194304,    4194305,
      8388607,    8388608,    8388609,    16777215,   16777216,   16777217,
      33554431,   33554432,   33554433,   67108863,   67108864,   67108865,
      134217727,  134217728,  134217729,  268435455,  268435456,  268435457,
      536870911,  536870912,  536870913,  1073741823, 1073741824, 1073741825,
      2147483647, 2147483648, 2147483649, 4294967295};

  uint32_t expected_outputs[] = {
      1,          1,          2,          4,          4,          8,
      8,          8,          16,         16,         16,         32,
      32,         32,         64,         64,         64,         128,
      128,        128,        256,        256,        256,        512,
      512,        512,        1024,       1024,       1024,       2048,
      2048,       2048,       4096,       4096,       4096,       8192,
      8192,       8192,       16384,      16384,      16384,      32768,
      32768,      32768,      65536,      65536,      65536,      131072,
      131072,     131072,     262144,     262144,     262144,     524288,
      524288,     524288,     1048576,    1048576,    1048576,    2097152,
      2097152,    2097152,    4194304,    4194304,    4194304,    8388608,
      8388608,    8388608,    16777216,   16777216,   16777216,   33554432,
      33554432,   33554432,   67108864,   67108864,   67108864,   134217728,
      134217728,  134217728,  268435456,  268435456,  268435456,  536870912,
      536870912,  536870912,  1073741824, 1073741824, 1073741824, 2147483648,
      2147483648, 2147483648, 2147483648, 2147483648};

  int len = sizeof(test_inputs) / sizeof(uint32_t);
  REQUIRE_EQ(len, sizeof(expected_outputs) / sizeof(uint32_t));

  for (int i = 0; i < len; ++i) {
    uint32_t r = find_nearest_gte_power_of_two(test_inputs[i]);
    REQUIRE_EQ(r, expected_outputs[i]);
  }
}

extern uint32_t nearest_ceil_log2(uint32_t input);

TEST(cmempools, nearest_ceil_log2) {
  // Hand-crafted the following input and expected output arrays, as we don't
  // want to depend on a math lib to do the testing.

  uint32_t test_inputs[] = {
      0,          1,          2,          3,          4,          5,
      7,          8,          9,          15,         16,         17,
      31,         32,         33,         63,         64,         65,
      127,        128,        129,        255,        256,        257,
      511,        512,        513,        1023,       1024,       1025,
      2047,       2048,       2049,       4095,       4096,       4097,
      8191,       8192,       8193,       16383,      16384,      16385,
      32767,      32768,      32769,      65535,      65536,      65537,
      131071,     131072,     131073,     262143,     262144,     262145,
      524287,     524288,     524289,     1048575,    1048576,    1048577,
      2097151,    2097152,    2097153,    4194303,    4194304,    4194305,
      8388607,    8388608,    8388609,    16777215,   16777216,   16777217,
      33554431,   33554432,   33554433,   67108863,   67108864,   67108865,
      134217727,  134217728,  134217729,  268435455,  268435456,  268435457,
      536870911,  536870912,  536870913,  1073741823, 1073741824, 1073741825,
      2147483647, 2147483648, 2147483649, 4294967295};

  uint32_t expected_outputs[] = {
      0,  0,  1,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,
      6,  7,  7,  7,  8,  8,  8,  9,  9,  9,  10, 10, 10, 11, 11, 11,
      12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17,
      17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22,
      22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27,
      28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 31, 31};

  int len = sizeof(test_inputs) / sizeof(uint32_t);
  REQUIRE_EQ(len, sizeof(expected_outputs) / sizeof(uint32_t));

  for (int i = 0; i < len; ++i) {
    uint32_t r = nearest_ceil_log2(test_inputs[i]);
    REQUIRE_EQ(r, expected_outputs[i]);
  }
}

TEST(cmempools, create_fails) {
  mempool* mp = mempool_create(0, sizeof(int), false);
  REQUIRE_EQ((void*)mp, NULL);

  mp = mempool_create(16, 0, false);
  REQUIRE_EQ((void*)mp, NULL);
}

TEST(cmempools, create_succeeds) {
  mempool* mp = mempool_create(256, sizeof(int), false);
  REQUIRE_NE((void*)mp, NULL);

  mempool_destroy(mp);
  REQUIRE_EQ((void*)mp, NULL);
}

TEST(cmempools, allocations_and_deallocations_fallback_disabled) {
  mempool* mp = mempool_create(256, sizeof(int), false);
  REQUIRE_NE((void*)mp, NULL);

  REQUIRE_EQ(mempool_used_count(mp), 0);
  REQUIRE_EQ(mempool_total_capacity(mp), 256);

  int* ptrs[256] = {0};

  // Allocate all buffers.
  for (uint32_t i = 0; i < 256; ++i) {
    ptrs[i] = mempool_alloc_entry(mp);
    REQUIRE_NE((void*)ptrs[i], NULL);
    *ptrs[i] = i;
    REQUIRE_EQ(mempool_used_count(mp), i + 1);
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  // Verify the failure to allocate further.
  int* tmp_ptr = mempool_alloc_entry(mp);
  REQUIRE_EQ((void*)tmp_ptr, NULL);

  // Deallocate all.
  for (uint32_t i = 0; i < 256; ++i) {
    mempool_free_entry(ptrs[i]);
    REQUIRE_EQ(mempool_used_count(mp), 256 - (i + 1));
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  // Reallocate all buffers.
  for (uint32_t i = 0; i < 256; ++i) {
    ptrs[i] = mempool_alloc_entry(mp);
    REQUIRE_NE((void*)ptrs[i], NULL);
    *ptrs[i] = i;
    REQUIRE_EQ(mempool_used_count(mp), i + 1);
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  // Verify the failure to allocate further again.
  tmp_ptr = mempool_alloc_entry(mp);
  REQUIRE_EQ((void*)tmp_ptr, NULL);

  // Deallocate all.
  for (uint32_t i = 0; i < 256; ++i) {
    mempool_free_entry(ptrs[i]);
    REQUIRE_EQ(mempool_used_count(mp), 256 - (i + 1));
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  mempool_destroy(mp);
  REQUIRE_EQ((void*)mp, NULL);
}

TEST(cmempools, allocations_and_deallocations_fallback_enabled) {
  mempool* mp = mempool_create(256, sizeof(int), true);
  REQUIRE_NE((void*)mp, NULL);

  REQUIRE_EQ(mempool_used_count(mp), 0);
  REQUIRE_EQ(mempool_total_capacity(mp), 256);

  int* ptrs[256] = {0};

  // Allocate all buffers.
  for (uint32_t i = 0; i < 256; ++i) {
    ptrs[i] = mempool_alloc_entry(mp);
    REQUIRE_NE((void*)ptrs[i], NULL);
    *ptrs[i] = i;
    REQUIRE_EQ(mempool_used_count(mp), i + 1);
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  // Verify the allocation of a buffer via fallback.
  int* tmp_ptr = mempool_alloc_entry(mp);
  REQUIRE_NE((void*)tmp_ptr, NULL);
  // Free the fallback buffer.
  mempool_free_entry(tmp_ptr);

  // Deallocate all.
  for (uint32_t i = 0; i < 256; ++i) {
    mempool_free_entry(ptrs[i]);
    REQUIRE_EQ(mempool_used_count(mp), 256 - (i + 1));
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  // Reallocate all buffers.
  for (uint32_t i = 0; i < 256; ++i) {
    ptrs[i] = mempool_alloc_entry(mp);
    REQUIRE_NE((void*)ptrs[i], NULL);
    *ptrs[i] = i;
    REQUIRE_EQ(mempool_used_count(mp), i + 1);
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  // Verify the allocation of a buffer via fallback again.
  tmp_ptr = mempool_alloc_entry(mp);
  REQUIRE_NE((void*)tmp_ptr, NULL);
  // Free the fallback buffer.
  mempool_free_entry(tmp_ptr);

  // Deallocate all.
  for (uint32_t i = 0; i < 256; ++i) {
    mempool_free_entry(ptrs[i]);
    REQUIRE_EQ(mempool_used_count(mp), 256 - (i + 1));
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  mempool_destroy(mp);
  REQUIRE_EQ((void*)mp, NULL);
}

TEST(cmempools, c_allocations_and_deallocations_fallback_disabled) {
  mempool* mp = mempool_create(256, sizeof(int), false);
  REQUIRE_NE((void*)mp, NULL);

  REQUIRE_EQ(mempool_used_count(mp), 0);
  REQUIRE_EQ(mempool_total_capacity(mp), 256);

  int* ptrs[256] = {0};

  // Allocate all buffers.
  for (uint32_t i = 0; i < 256; ++i) {
    ptrs[i] = mempool_calloc_entry(mp);
    REQUIRE_NE((void*)ptrs[i], NULL);
    *ptrs[i] = i;
    REQUIRE_EQ(mempool_used_count(mp), i + 1);
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  // Verify the failure to allocate further.
  int* tmp_ptr = mempool_calloc_entry(mp);
  REQUIRE_EQ((void*)tmp_ptr, NULL);

  // Deallocate all.
  for (uint32_t i = 0; i < 256; ++i) {
    mempool_free_entry(ptrs[i]);
    REQUIRE_EQ(mempool_used_count(mp), 256 - (i + 1));
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  // Reallocate all buffers.
  for (uint32_t i = 0; i < 256; ++i) {
    ptrs[i] = mempool_calloc_entry(mp);
    REQUIRE_NE((void*)ptrs[i], NULL);
    *ptrs[i] = i;
    REQUIRE_EQ(mempool_used_count(mp), i + 1);
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  // Verify the failure to allocate further again.
  tmp_ptr = mempool_calloc_entry(mp);
  REQUIRE_EQ((void*)tmp_ptr, NULL);

  // Deallocate all.
  for (uint32_t i = 0; i < 256; ++i) {
    mempool_free_entry(ptrs[i]);
    REQUIRE_EQ(mempool_used_count(mp), 256 - (i + 1));
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  mempool_destroy(mp);
  REQUIRE_EQ((void*)mp, NULL);
}

TEST(cmempools, c_allocations_and_deallocations_fallback_enabled) {
  mempool* mp = mempool_create(256, sizeof(int), true);
  REQUIRE_NE((void*)mp, NULL);

  REQUIRE_EQ(mempool_used_count(mp), 0);
  REQUIRE_EQ(mempool_total_capacity(mp), 256);

  int* ptrs[256] = {0};

  // Allocate all buffers.
  for (uint32_t i = 0; i < 256; ++i) {
    ptrs[i] = mempool_calloc_entry(mp);
    REQUIRE_NE((void*)ptrs[i], NULL);
    *ptrs[i] = i;
    REQUIRE_EQ(mempool_used_count(mp), i + 1);
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  // Verify the allocation of a buffer via fallback.
  int* tmp_ptr = mempool_calloc_entry(mp);
  REQUIRE_NE((void*)tmp_ptr, NULL);
  // Free the fallback buffer.
  mempool_free_entry(tmp_ptr);

  // Deallocate all.
  for (uint32_t i = 0; i < 256; ++i) {
    mempool_free_entry(ptrs[i]);
    REQUIRE_EQ(mempool_used_count(mp), 256 - (i + 1));
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  // Reallocate all buffers.
  for (uint32_t i = 0; i < 256; ++i) {
    ptrs[i] = mempool_calloc_entry(mp);
    REQUIRE_NE((void*)ptrs[i], NULL);
    *ptrs[i] = i;
    REQUIRE_EQ(mempool_used_count(mp), i + 1);
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  // Verify the allocation of a buffer via fallback again.
  tmp_ptr = mempool_calloc_entry(mp);
  REQUIRE_NE((void*)tmp_ptr, NULL);
  // Free the fallback buffer.
  mempool_free_entry(tmp_ptr);

  // Deallocate all.
  for (uint32_t i = 0; i < 256; ++i) {
    mempool_free_entry(ptrs[i]);
    REQUIRE_EQ(mempool_used_count(mp), 256 - (i + 1));
    REQUIRE_EQ(mempool_total_capacity(mp), 256);
  }

  mempool_destroy(mp);
  REQUIRE_EQ((void*)mp, NULL);
}

// C_R_MEMPOOL TESTS

TEST(r_mempools, create_fails) {
  r_mempool* rmp = r_mempool_create(16, 131072, 0, fallback_disabled);
  REQUIRE_EQ((void*)rmp, NULL);

  rmp = r_mempool_create(16, 0, 131072, fallback_disabled);
  REQUIRE_EQ((void*)rmp, NULL);

  rmp = r_mempool_create(0, 131072, 131072, fallback_disabled);
  REQUIRE_EQ((void*)rmp, NULL);

  rmp = r_mempool_create(16, 8, 131072, fallback_disabled);
  REQUIRE_EQ((void*)rmp, NULL);

  rmp = r_mempool_create(16, 16, 131072, fallback_disabled);
  REQUIRE_EQ((void*)rmp, NULL);

  rmp = r_mempool_create(16, 64, 131072, -1);
  REQUIRE_EQ((void*)rmp, NULL);

  rmp = r_mempool_create(16, 64, 131072, __fallback_end_place_holder);
  REQUIRE_EQ((void*)rmp, NULL);

  rmp = r_mempool_create(16, 64, 131072, __fallback_end_place_holder + 1);
  REQUIRE_EQ((void*)rmp, NULL);
}

TEST(r_mempools, create_succeeds) {
  r_mempool* rmp = r_mempool_create(16, 131072, 131072, fallback_disabled);
  REQUIRE_NE((void*)rmp, NULL);

  r_mempool_destroy(rmp);
  REQUIRE_EQ((void*)rmp, NULL);
}

TEST(r_mempools, simple_allocations) {
  r_mempool* rmp = r_mempool_create(16, 64, 128, fallback_disabled);
  char* ptr = NULL;

  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  ptr = r_mempool_alloc_entry(rmp, 1);  // Should come from buffers of 16
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  ptr = r_mempool_alloc_entry(rmp, 16);  // Should come from buffers of 16
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  ptr = r_mempool_alloc_entry(rmp, 17);  // Should come from buffers of 32
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  ptr = r_mempool_alloc_entry(rmp, 32);  // Should come from buffers of 32
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 64), 0);
  ptr = r_mempool_alloc_entry(rmp, 33);  // Should come from buffers of 64
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 64), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 64), 0);
  ptr = r_mempool_alloc_entry(rmp, 63);  // Should come from buffers of 64
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 64), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 64), 0);
  ptr = r_mempool_alloc_entry(rmp, 64);  // Should come from buffers of 64
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 64), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ((void*)r_mempool_alloc_entry(rmp, 0), NULL);
  REQUIRE_EQ((void*)r_mempool_alloc_entry(rmp, 65), NULL);

  r_mempool_destroy(rmp);
}

TEST(r_mempools, simple_reallocations) {
  r_mempool* rmp = r_mempool_create(16, 64, 128, fallback_disabled);
  char* ptr = NULL;

  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  ptr = r_mempool_realloc_entry(rmp, ptr, 3);
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 1);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);

  for (int i = 0; i < 3; ++i) {
    ptr[i] = i;
  }

  char* orig = ptr;
  ptr = r_mempool_realloc_entry(rmp, ptr, 6);
  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 1);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  // Since the block size is still 16 no real "reallocation" happened
  REQUIRE_EQ((void*)orig, (void*)ptr);
  for (int i = 0; i < 6; ++i) {
    if (i < 3) {
      REQUIRE_EQ(ptr[i], i);
    } else {
      ptr[i] = i;
    }
  }

  ptr = r_mempool_realloc_entry(rmp, ptr, 20);
  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 1);
  REQUIRE_NE((void*)orig, (void*)ptr);
  for (int i = 0; i < 20; ++i) {
    if (i < 6) {
      REQUIRE_EQ(ptr[i], i);
    } else {
      ptr[i] = i;
    }
  }

  orig = ptr;
  ptr = r_mempool_realloc_entry(rmp, ptr, 5);
  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 1);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  REQUIRE_NE((void*)orig, (void*)ptr);
  for (int i = 0; i < 5; ++i) {
    REQUIRE_EQ(ptr[i], i);
  }

  r_mempool_destroy(rmp);
}

TEST(r_mempools, simple_c_allocations) {
  r_mempool* rmp = r_mempool_create(16, 64, 128, fallback_disabled);
  char* ptr = NULL;

  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  ptr = r_mempool_calloc_entry(rmp, 1);  // Should come from buffers of 16
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  ptr = r_mempool_calloc_entry(rmp, 16);  // Should come from buffers of 16
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  ptr = r_mempool_calloc_entry(rmp, 17);  // Should come from buffers of 32
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  ptr = r_mempool_calloc_entry(rmp, 32);  // Should come from buffers of 32
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 64), 0);
  ptr = r_mempool_calloc_entry(rmp, 33);  // Should come from buffers of 64
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 64), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 64), 0);
  ptr = r_mempool_calloc_entry(rmp, 63);  // Should come from buffers of 64
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 64), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 64), 0);
  ptr = r_mempool_calloc_entry(rmp, 64);  // Should come from buffers of 64
  REQUIRE_NE((void*)ptr, NULL);
  REQUIRE_EQ(r_mempool_used_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_used_count(rmp, 64), 1);
  r_mempool_free_entry(ptr);

  REQUIRE_EQ((void*)r_mempool_calloc_entry(rmp, 0), NULL);
  REQUIRE_EQ((void*)r_mempool_calloc_entry(rmp, 65), NULL);

  r_mempool_destroy(rmp);
}

TEST(r_mempools, exhaust_all_fallback_disabled) {
  r_mempool* rmp = r_mempool_create(16, 64, 128, fallback_disabled);

  uint32_t ptrs_len = 224;  // 16: 128, 32: 64, 64: 32 => 128 + 64 + 32 = 224

  void* ptrs[ptrs_len];

  for (uint32_t i = 0; i < ptrs_len; ++i) {
    ptrs[i] = r_mempool_alloc_entry(rmp, 8);
    REQUIRE_NE((void*)ptrs[i], NULL);
    // It even gives 32 and 64 sized buffers to fulfill our request.
  }

  for (uint32_t size = 16; size <= 64; size *= 2) {
    REQUIRE_EQ(r_mempool_used_count(rmp, size),
               r_mempool_total_capacity(rmp, size));
  }

  // It doesn't allocate new memory from the heap, once the pre-allocated
  // buffers are exhausted, that's it, no new memory till some are given
  // back.
  void* tmp_ptr = r_mempool_alloc_entry(rmp, 8);
  REQUIRE_EQ((void*)tmp_ptr, NULL);

  for (uint32_t i = 0; i < ptrs_len; ++i) {
    r_mempool_free_entry(ptrs[i]);
  }

  for (uint32_t size = 16; size <= 64; size *= 2) {
    REQUIRE_EQ(r_mempool_used_count(rmp, size), 0);
  }

  r_mempool_destroy(rmp);
}

TEST(r_mempools, c_exhaust_all_fallback_disabled) {
  r_mempool* rmp = r_mempool_create(16, 64, 128, fallback_disabled);

  uint32_t ptrs_len = 224;  // 16: 128, 32: 64, 64: 32 => 128 + 64 + 32 = 224

  void* ptrs[ptrs_len];

  for (uint32_t i = 0; i < ptrs_len; ++i) {
    ptrs[i] = r_mempool_calloc_entry(rmp, 8);
    REQUIRE_NE((void*)ptrs[i], NULL);
    // It even gives 32 and 64 sized buffers to fulfill our request.
  }

  for (uint32_t size = 16; size <= 64; size *= 2) {
    REQUIRE_EQ(r_mempool_used_count(rmp, size),
               r_mempool_total_capacity(rmp, size));
  }

  // It doesn't allocate new memory from the heap, once the pre-allocated
  // buffers are exhausted, that's it, no new memory till some are given
  // back.
  void* tmp_ptr = r_mempool_calloc_entry(rmp, 8);
  REQUIRE_EQ((void*)tmp_ptr, NULL);

  for (uint32_t i = 0; i < ptrs_len; ++i) {
    r_mempool_free_entry(ptrs[i]);
  }

  for (uint32_t size = 16; size <= 64; size *= 2) {
    REQUIRE_EQ(r_mempool_used_count(rmp, size), 0);
  }

  r_mempool_destroy(rmp);
}

TEST(r_mempools, try_exhausting_with_fallback_at_first_exhaustion) {
  r_mempool* rmp = r_mempool_create(16, 64, 128, fallback_at_first_exhaustion);

  uint32_t ptrs_len = 224;  // 16: 128, 32: 64, 64: 32 => 128 + 64 + 32 = 224

  void* ptrs[ptrs_len];
  uint ptr_iterator = 0;

  for (uint32_t i = 0; i < 128; ++i) {
    ptrs[ptr_iterator++] = r_mempool_alloc_entry(rmp, 16);
  }

  for (uint32_t i = 0; i < 64; ++i) {
    ptrs[ptr_iterator++] = r_mempool_alloc_entry(rmp, 32);
  }

  for (uint32_t i = 0; i < 32; ++i) {
    ptrs[ptr_iterator++] = r_mempool_alloc_entry(rmp, 64);
  }

  for (uint32_t size = 16; size <= 64; size *= 2) {
    REQUIRE_EQ(r_mempool_used_count(rmp, size),
               r_mempool_total_capacity(rmp, size));
  }

  // Even if the pre-allocated buffers have been exhausted, as the
  // fallback is enabled, it will allocate more memory from heap
  // to fulfill the ask. The allocations will happen via separate
  // internal memory pools, and therefore the function
  // r_mempool_alloc_entry will return distinct values for different
  // sizes.
  void* tmp_ptr_16 = r_mempool_alloc_entry(rmp, 16);
  REQUIRE_NE((void*)tmp_ptr_16, NULL);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 1);

  void* tmp_ptr_32 = r_mempool_alloc_entry(rmp, 32);
  REQUIRE_NE((void*)tmp_ptr_32, NULL);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 1);

  void* tmp_ptr_64 = r_mempool_alloc_entry(rmp, 64);
  REQUIRE_NE((void*)tmp_ptr_64, NULL);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 1);

  for (uint32_t i = 0; i < ptrs_len; ++i) {
    r_mempool_free_entry(ptrs[i]);
  }

  for (uint32_t size = 16; size <= 64; size *= 2) {
    REQUIRE_EQ(r_mempool_used_count(rmp, size), 0);
  }

  r_mempool_free_entry(tmp_ptr_16);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 0);

  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 1);
  r_mempool_free_entry(tmp_ptr_32);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 0);

  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 1);
  r_mempool_free_entry(tmp_ptr_64);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 0);

  r_mempool_destroy(rmp);
}

TEST(r_mempools, c_try_exhausting_with_fallback_at_first_exhaustion) {
  r_mempool* rmp = r_mempool_create(16, 64, 128, fallback_at_first_exhaustion);

  uint32_t ptrs_len = 224;  // 16: 128, 32: 64, 64: 32 => 128 + 64 + 32 = 224

  void* ptrs[ptrs_len];
  uint ptr_iterator = 0;

  for (uint32_t i = 0; i < 128; ++i) {
    ptrs[ptr_iterator++] = r_mempool_calloc_entry(rmp, 16);
  }

  for (uint32_t i = 0; i < 64; ++i) {
    ptrs[ptr_iterator++] = r_mempool_calloc_entry(rmp, 32);
  }

  for (uint32_t i = 0; i < 32; ++i) {
    ptrs[ptr_iterator++] = r_mempool_calloc_entry(rmp, 64);
  }

  for (uint32_t size = 16; size <= 64; size *= 2) {
    REQUIRE_EQ(r_mempool_used_count(rmp, size),
               r_mempool_total_capacity(rmp, size));
  }

  // Even if the pre-allocated buffers have been exhausted, as the
  // fallback is enabled, it will allocate more memory from heap
  // to fulfill the ask. The allocations will happen via separate
  // internal memory pools, and therefore the function
  // r_mempool_calloc_entry will return distinct values for different
  // sizes.
  void* tmp_ptr_16 = r_mempool_calloc_entry(rmp, 16);
  REQUIRE_NE((void*)tmp_ptr_16, NULL);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 1);

  void* tmp_ptr_32 = r_mempool_calloc_entry(rmp, 32);
  REQUIRE_NE((void*)tmp_ptr_32, NULL);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 1);

  void* tmp_ptr_64 = r_mempool_calloc_entry(rmp, 64);
  REQUIRE_NE((void*)tmp_ptr_64, NULL);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 1);

  for (uint32_t i = 0; i < ptrs_len; ++i) {
    r_mempool_free_entry(ptrs[i]);
  }

  for (uint32_t size = 16; size <= 64; size *= 2) {
    REQUIRE_EQ(r_mempool_used_count(rmp, size), 0);
  }

  r_mempool_free_entry(tmp_ptr_16);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 0);

  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 1);
  r_mempool_free_entry(tmp_ptr_32);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 0);

  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 1);
  r_mempool_free_entry(tmp_ptr_64);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 0);

  r_mempool_destroy(rmp);
}

TEST(r_mempools, try_exhausting_with_fallback_at_last_exhaustion) {
  r_mempool* rmp = r_mempool_create(16, 64, 128, fallback_at_last_exhaustion);

  uint32_t ptrs_len = 224;  // 16: 128, 32: 64, 64: 32 => 128 + 64 + 32 = 224

  void* ptrs[ptrs_len];
  uint ptr_iterator = 0;

  for (uint32_t i = 0; i < 128; ++i) {
    ptrs[ptr_iterator++] = r_mempool_alloc_entry(rmp, 16);
  }

  for (uint32_t i = 0; i < 64; ++i) {
    ptrs[ptr_iterator++] = r_mempool_alloc_entry(rmp, 32);
  }

  for (uint32_t i = 0; i < 32; ++i) {
    ptrs[ptr_iterator++] = r_mempool_alloc_entry(rmp, 64);
  }

  for (uint32_t size = 16; size <= 64; size *= 2) {
    REQUIRE_EQ(r_mempool_used_count(rmp, size),
               r_mempool_total_capacity(rmp, size));
  }

  // Even if the pre-allocated buffers have been exhausted, as the
  // fallback is enabled, it will allocate more memory from heap
  // to fulfill the ask.
  // Please notice that the r_mempool_dynamic_allocs_count will
  // climb up regardless of the size.
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 0);
  void* tmp_ptr_16 = r_mempool_alloc_entry(rmp, 16);
  REQUIRE_NE((void*)tmp_ptr_16, NULL);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 1);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 1);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 1);

  void* tmp_ptr_32 = r_mempool_alloc_entry(rmp, 32);
  REQUIRE_NE((void*)tmp_ptr_32, NULL);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 2);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 2);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 2);

  void* tmp_ptr_64 = r_mempool_alloc_entry(rmp, 64);
  REQUIRE_NE((void*)tmp_ptr_64, NULL);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 3);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 3);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 3);

  for (uint32_t i = 0; i < ptrs_len; ++i) {
    r_mempool_free_entry(ptrs[i]);
  }

  for (uint32_t size = 16; size <= 64; size *= 2) {
    REQUIRE_EQ(r_mempool_used_count(rmp, size), 0);
  }

  // The r_mempool_dynamic_allocs_count still holds the cumulative
  // dynamic allocation number regardless of the size
  r_mempool_free_entry(tmp_ptr_16);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 2);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 2);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 2);
  r_mempool_free_entry(tmp_ptr_32);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 1);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 1);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 1);
  r_mempool_free_entry(tmp_ptr_64);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 0);

  r_mempool_destroy(rmp);
}

TEST(r_mempools, c_try_exhausting_with_fallback_at_last_exhaustion) {
  r_mempool* rmp = r_mempool_create(16, 64, 128, fallback_at_last_exhaustion);

  uint32_t ptrs_len = 224;  // 16: 128, 32: 64, 64: 32 => 128 + 64 + 32 = 224

  void* ptrs[ptrs_len];
  uint ptr_iterator = 0;

  for (uint32_t i = 0; i < 128; ++i) {
    ptrs[ptr_iterator++] = r_mempool_calloc_entry(rmp, 16);
  }

  for (uint32_t i = 0; i < 64; ++i) {
    ptrs[ptr_iterator++] = r_mempool_calloc_entry(rmp, 32);
  }

  for (uint32_t i = 0; i < 32; ++i) {
    ptrs[ptr_iterator++] = r_mempool_calloc_entry(rmp, 64);
  }

  for (uint32_t size = 16; size <= 64; size *= 2) {
    REQUIRE_EQ(r_mempool_used_count(rmp, size),
               r_mempool_total_capacity(rmp, size));
  }

  // Even if the pre-allocated buffers have been exhausted, as the
  // fallback is enabled, it will allocate more memory from heap
  // to fulfill the ask.
  // Please notice that the r_mempool_dynamic_allocs_count will
  // climb up regardless of the size.
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 0);
  void* tmp_ptr_16 = r_mempool_calloc_entry(rmp, 16);
  REQUIRE_NE((void*)tmp_ptr_16, NULL);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 1);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 1);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 1);

  void* tmp_ptr_32 = r_mempool_calloc_entry(rmp, 32);
  REQUIRE_NE((void*)tmp_ptr_32, NULL);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 2);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 2);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 2);

  void* tmp_ptr_64 = r_mempool_calloc_entry(rmp, 64);
  REQUIRE_NE((void*)tmp_ptr_64, NULL);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 3);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 3);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 3);

  for (uint32_t i = 0; i < ptrs_len; ++i) {
    r_mempool_free_entry(ptrs[i]);
  }

  for (uint32_t size = 16; size <= 64; size *= 2) {
    REQUIRE_EQ(r_mempool_used_count(rmp, size), 0);
  }

  // The r_mempool_dynamic_allocs_count still holds the cumulative
  // dynamic allocation number regardless of the size
  r_mempool_free_entry(tmp_ptr_16);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 2);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 2);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 2);
  r_mempool_free_entry(tmp_ptr_32);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 1);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 1);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 1);
  r_mempool_free_entry(tmp_ptr_64);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 16), 0);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 32), 0);
  REQUIRE_EQ(r_mempool_dynamic_allocs_count(rmp, 64), 0);

  r_mempool_destroy(rmp);
}

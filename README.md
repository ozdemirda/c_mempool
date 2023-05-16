A humble collection of memory management functions implemented in C that
provide `ordinary` and `ranged` `memory pools`.

The idea is quite simple, some memory blocks get preallocated and are
accummulated in a pool. When the program needs a memory block and asks
for it, it gets it from the pool with O(1) complexity. Giving the memory
back too has O(1) complexity. While the capacity of a memory pool is
limited, depending on the chosen `fallback` policy, it may still hand
some more blocks from the underlying memory management system or it
may simply return NULL when the blocks were exhausted.

A ranged memory pool can be defined as a collection of simple memory
pools with variety of element sizes and numbers.

A simple example always comes in handy:

```c
#include <cmempool.h>
#include <stdlib.h>

int *ptrs[1024] = {0};

int main() {
    // Let's create an ordinary memory pool that contains
    // 1024 integers, and does not fallback to the OS when
    // it's exhausted. If the third argument was `true` in
    // the following expression, the memory pool would try
    // to allocate memory via dynamic memory management
    // mechanisms, once its own slots were exhausted.
    mempool *mp = mempool_create(1024, sizeof(int), false);
    if (!mp) {
        // Failed to create a new memory pool!
        exit(EXIT_FAILURE);
    }

    // The following allocations within the for loop
    // are "guaranteed" to be successfull. Even with
    // overcommitting in place, since the memory buffers
    // are written into while getting initialized,
    // they will be mapped into the memory space of this
    // process.
    for (int i = 0; i < 1024; ++i) {
        ptrs[i] = (int*) mempool_alloc_entry(mp);
        *ptrs[i] = i;
    }

    // The following call to 'mempool_alloc_entry' will return NULL, as
    // the fallback argument to 'mempool_create' was 'false'. If that
    // was 'true', we most probably would get a slot from the OS.
    int *tmp_ptr = (int*) mempool_alloc_entry(mp);

    // The memory slot can be 'released' using the 'mempool_free_entry' macro
    for (int i = 0; i < 1024; ++i) {
        mempool_free_entry(ptrs[i]);
    }

    // Please notice that, the macro mempool_free_entry will return the
    // slot back to the pool to be reused later (if it was allocated from
    // the pool in the first place which is the case in our example, as we
    // didn't 'fallback' to the dynamic memory allocation mechanisms). To
    // really 'free' the underlying buffers, one has to call mempool_destroy.
    mempool_destroy(mp);

    return EXIT_SUCCESS;
}
```

And an example utilizing `ranged` memory pools:

```c
#include <cmempool.h>
#include <stdlib.h>
#include <stdio.h>

int *ptrs[1024] = {0};

int main() {
    // Let's create a `ranged` memory pool which will allocate
    // a collection of memory pools with the following layout:
    // ---------------------------------------------
    // Size of the element        Number of elements
    // ---------------------------------------------
    //     16                         131072
    //     32                         65536
    //     64                         32768
    //     128                        16384
    //     256                        8192
    //     512                        4096
    //     1024                       2048
    //     2048                       1024
    //     4096                       512
    //     8192                       256
    //     16384                      128
    //     32768                      64
    //     65536                      32
    // ---------------------------------------------
    // Please also notice the fourth argument which
    // tells the ranged memory pool to fallback to the
    // OS when the ranged memory pool fails to find an
    // available slot in the corresponding internal memory
    // pool. If this argument was fallback_at_last_exhaustion,
    // the ranged memory pool would try to fulfill the ask
    // via its next larger internal memory pools, and it
    // would only fall back to the OS when no internal
    // buffers were available.
    r_mempool *rmp = r_mempool_create(16, 65536, 131072,
        fallback_at_first_exhaustion);
    if (!rmp) {
        // Failed to create a new memory pool!
        exit(EXIT_FAILURE);
    }

    // The following function call will return a preallocated buffer
    // from the pool of slots that are 32 bytes long, since
    // pow(2, ceil(log2(24))) is 32.
    char *str_ptr = (char*)r_mempool_alloc_entry(rmp, 24);

    snprintf(str_ptr, 24, "Hello world!\n");
    printf("%p => %s", str_ptr, str_ptr);

    // Let's extend the memory block, it should still contain
    // the same data, but the pointer will contain a different
    // address.
    str_ptr = r_mempool_realloc_entry(rmp, str_ptr, 48);
    printf("%p => %s", str_ptr, str_ptr);

    // Let's return the slot back to the pool.
    r_mempool_free_entry(str_ptr);

    // Let's destroy the ranged memory pool.
    r_mempool_destroy(rmp);

    return EXIT_SUCCESS;
}
```

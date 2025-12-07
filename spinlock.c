#include <stdatomic.h>

void acquire(atomic_flag *lock) {
    while (atomic_flag_test_and_set_explicit(lock, memory_order_acquire)) {
        __builtin_ia32_pause();
    }
}

void release(atomic_flag *lock) {
    atomic_flag_test_and_set_explicit(lock, memory_order_release);
}
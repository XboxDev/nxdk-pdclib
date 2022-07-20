/* Atomics <stdatomic.h>

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

#ifndef _PDCLIB_STDATOMIC_H
#define _PDCLIB_STDATOMIC_H _PDCLIB_STDATOMIC_H

#ifndef __clang__
#error This implementation of stdatomic.h requires the Clang compiler!
#endif

#include <pdclib/_PDCLIB_int.h>

#define ATOMIC_BOOL_LOCK_FREE __CLANG_ATOMIC_BOOL_LOCK_FREE
#define ATOMIC_CHAR_LOCK_FREE __CLANG_ATOMIC_CHAR_LOCK_FREE
#define ATOMIC_CHAR16_T_LOCK_FREE __CLANG_ATOMIC_CHAR16_T_LOCK_FREE
#define ATOMIC_CHAR32_T_LOCK_FREE __CLANG_ATOMIC_CHAR32_T_LOCK_FREE
#define ATOMIC_WCHAR_T_LOCK_FREE __CLANG_ATOMIC_WCHAR_T_LOCK_FREE
#define ATOMIC_SHORT_LOCK_FREE __CLANG_ATOMIC_SHORT_LOCK_FREE
#define ATOMIC_INT_LOCK_FREE __CLANG_ATOMIC_INT_LOCK_FREE
#define ATOMIC_LONG_LOCK_FREE __CLANG_ATOMIC_LONG_LOCK_FREE
#define ATOMIC_LLONG_LOCK_FREE __CLANG_ATOMIC_LLONG_LOCK_FREE
#define ATOMIC_POINTER_LOCK_FREE __CLANG_ATOMIC_POINTER_LOCK_FREE

// 7.17.3
typedef enum
{
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_consume = __ATOMIC_CONSUME,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST,
} memory_order;

// 7.17.2.1
#define ATOMIC_VAR_INIT(value) (value)

// 7.17.2.2
#define atomic_init(obj, value) __c11_atomic_init(obj, value)

// 7.17.3.1
#define kill_dependency(y) (y)

// 7.17.4.1
#define atomic_thread_fence(order) __c11_atomic_thread_fence(order)

// 7.17.4.2
#define atomic_signal_fence(order) __c11_atomic_signal_fence(order)

// 7.17.5.1
#define atomic_is_lock_free(obj) __c11_atomic_is_lock_free(sizeof(*(obj)))

// 7.17.6
typedef _Atomic _Bool atomic_bool;
typedef _Atomic char atomic_char;
typedef _Atomic signed char atomic_schar;
typedef _Atomic unsigned char atomic_uchar;
typedef _Atomic short atomic_short;
typedef _Atomic unsigned short atomic_ushort;
typedef _Atomic int atomic_int;
typedef _Atomic unsigned int atomic_uint;
typedef _Atomic long atomic_long;
typedef _Atomic unsigned long atomic_ulong;
typedef _Atomic long long atomic_llong;
typedef _Atomic unsigned long long atomic_ullong;

// TODO: PDCLib does not provide the base types
//typedef _Atomic ? atomic_char16_t;
//typedef _Atomic ? atomic_char32_t;
typedef _Atomic _PDCLIB_wchar_t atomic_wchar_t;
typedef _Atomic _PDCLIB_int8_t atomic_int_least8_t;
typedef _Atomic _PDCLIB_uint8_t atomic_uint_least8_t;
typedef _Atomic _PDCLIB_int16_t atomic_int_least16_t;
typedef _Atomic _PDCLIB_uint16_t atomic_uint_least16_t;
typedef _Atomic _PDCLIB_int32_t atomic_int_least32_t;
typedef _Atomic _PDCLIB_uint32_t atomic_uint_least32_t;
typedef _Atomic _PDCLIB_int64_t atomic_int_least64_t;
typedef _Atomic _PDCLIB_uint64_t atomic_uint_least64_t;
typedef _Atomic _PDCLIB_int_fast8_t atomic_int_fast8_t;
typedef _Atomic _PDCLIB_uint_fast8_t atomic_uint_fast8_t;
typedef _Atomic _PDCLIB_int_fast16_t atomic_int_fast16_t;
typedef _Atomic _PDCLIB_uint_fast16_t atomic_uint_fast16_t;
typedef _Atomic _PDCLIB_int_fast32_t atomic_int_fast32_t;
typedef _Atomic _PDCLIB_uint_fast32_t atomic_uint_fast32_t;
typedef _Atomic _PDCLIB_int_fast64_t atomic_int_fast64_t;
typedef _Atomic _PDCLIB_uint_fast64_t atomic_uint_fast64_t;
typedef _Atomic _PDCLIB_intptr_t atomic_intptr_t;
typedef _Atomic _PDCLIB_uintptr_t atomic_uintptr_t;
typedef _Atomic _PDCLIB_size_t atomic_size_t;
typedef _Atomic _PDCLIB_ptrdiff_t  atomic_ptrdiff_t;
typedef _Atomic _PDCLIB_intmax_t atomic_intmax_t;
typedef _Atomic _PDCLIB_uintmax_t atomic_uintmax_t;

#define ATOMIC_FLAG_INIT { ATOMIC_VAR_INIT(FALSE) }

typedef struct
{
    atomic_bool __flag;
} atomic_flag;

// 7.17.7.1
#define atomic_store_explicit(object, desired, order) __c11_atomic_store(object, desired, order)
#define atomic_store(object, desired) atomic_store_explicit(object, desired, memory_order_seq_cst)

// 7.17.7.2
#define atomic_load_explicit(object, order) __c11_atomic_load(object, order)
#define atomic_load(object) atomic_load_explicit(object, memory_order_seq_cst)

// 7.17.7.3
#define atomic_exchange_explicit(object, desired, order) __c11_atomic_exchange(object, desired, order)
#define atomic_exchange(object, desired) atomic_exchange_explicit(object, desired, memory_order_seq_cst)

// 7.17.7.4
#define atomic_compare_exchange_strong_explicit(object, expected, desired, success, failure) __c11_atomic_compare_exchange_strong(object, expected, desired, success, failure)
#define atomic_compare_exchange_strong(object, expected, desired) atomic_compare_exchange_strong_explicit(object, expected, desired, memory_order_seq_cst, memory_order_seq_cst)
#define atomic_compare_exchange_weak_explicit(object, expected, desired, success, failure) __c11_atomic_compare_exchange_weak(object, expected, desired, success, failure)
#define atomic_compare_exchange_weak(object, expected, desired) atomic_compare_exchange_weak_explicit(object, expected, desired, memory_order_seq_cst, memory_order_seq_cst)

// 7.17.7.5
#define atomic_fetch_add(object, operand) __c11_atomic_fetch_add((object), (operand), memory_order_seq_cst)
#define atomic_fetch_add_explicit(object, operand, order) __c11_atomic_fetch_add((object), (operand), (order))
#define atomic_fetch_sub(object, operand) __c11_atomic_fetch_sub((object), (operand), memory_order_seq_cst)
#define atomic_fetch_sub_explicit(object, operand, order) __c11_atomic_fetch_sub((object), (operand), (order))
#define atomic_fetch_or(object, operand) __c11_atomic_fetch_or((object), (operand), memory_order_seq_cst)
#define atomic_fetch_or_explicit(object, operand, order) __c11_atomic_fetch_or((object), (operand), (order))
#define atomic_fetch_xor(object, operand) __c11_atomic_fetch_xor((object), (operand), memory_order_seq_cst)
#define atomic_fetch_xor_explicit(object, operand, order) __c11_atomic_fetch_xor((object), (operand), (order))
#define atomic_fetch_and(object, operand) __c11_atomic_fetch_and((object), (operand), memory_order_seq_cst)
#define atomic_fetch_and_explicit(object, operand, order) __c11_atomic_fetch_and((object), (operand), (order))

// 7.17.8.1
static inline _Bool atomic_flag_test_and_set_explicit(volatile atomic_flag *object, memory_order order)
{
    return (atomic_exchange_explicit(&object->__flag, 1, order));
}
static inline _Bool atomic_flag_test_and_set(volatile atomic_flag *object)
{
    return atomic_flag_test_and_set_explicit(object, memory_order_seq_cst);
}

// 7.17.8.2
static inline void atomic_flag_clear_explicit(volatile atomic_flag *object, memory_order order)
{
    atomic_store_explicit(&object->__flag, 0, order);
}
static inline void atomic_flag_clear(volatile atomic_flag *object)
{
    atomic_flag_clear_explicit(object, memory_order_seq_cst);
}

#endif

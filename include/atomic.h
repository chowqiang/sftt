/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _ATOMIC_H_
#define _ATOMIC_H_

/**
 * @file
 * Atomic Operations
 *
 * This file defines a generic API for atomic operations.
 */

#include <stdint.h>

/**
 * Compiler barrier.
 *
 * Guarantees that operation reordering does not occur at compile time
 * for operations directly before and after the barrier.
 */
#define	compiler_barrier() do {		\
	asm volatile ("" : : : "memory");	\
} while(0)

/*------------------------- 16 bit atomic operations -------------------------*/

/**
 * Atomic compare and set.
 *
 * (atomic) equivalent to:
 *   if (*dst == exp)
 *     *dst = src (all 16-bit words)
 *
 * @param dst
 *   The destination location into which the value will be written.
 * @param exp
 *   The expected value.
 * @param src
 *   The new value.
 * @return
 *   Non-zero on success; 0 on failure.
 */
static inline int
atomic16_cmpset(volatile uint16_t *dst, uint16_t exp, uint16_t src);

static inline int
atomic16_cmpset(volatile uint16_t *dst, uint16_t exp, uint16_t src)
{
	return __sync_bool_compare_and_swap(dst, exp, src);
}

/**
 * Atomic exchange.
 *
 * (atomic) equivalent to:
 *   ret = *dst
 *   *dst = val;
 *   return ret;
 *
 * @param dst
 *   The destination location into which the value will be written.
 * @param val
 *   The new value.
 * @return
 *   The original value at that location
 */
static inline uint16_t
atomic16_exchange(volatile uint16_t *dst, uint16_t val);

static inline uint16_t
atomic16_exchange(volatile uint16_t *dst, uint16_t val)
{
#if defined(__clang__)
	return __atomic_exchange_n(dst, val, __ATOMIC_SEQ_CST);
#else
	return __atomic_exchange_2(dst, val, __ATOMIC_SEQ_CST);
#endif
}

/**
 * The atomic counter structure.
 */
typedef struct {
	volatile int16_t cnt; /**< An internal counter value. */
} atomic16_t;

/**
 * Static initializer for an atomic counter.
 */
#define RTE_ATOMIC16_INIT(val) { (val) }

/**
 * Initialize an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
atomic16_init(atomic16_t *v)
{
	v->cnt = 0;
}

/**
 * Atomically read a 16-bit value from a counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   The value of the counter.
 */
static inline int16_t
atomic16_read(const atomic16_t *v)
{
	return v->cnt;
}

/**
 * Atomically set a counter to a 16-bit value.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param new_value
 *   The new value for the counter.
 */
static inline void
atomic16_set(atomic16_t *v, int16_t new_value)
{
	v->cnt = new_value;
}

/**
 * Atomically add a 16-bit value to an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 */
static inline void
atomic16_add(atomic16_t *v, int16_t inc)
{
	__sync_fetch_and_add(&v->cnt, inc);
}

/**
 * Atomically subtract a 16-bit value from an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 */
static inline void
atomic16_sub(atomic16_t *v, int16_t dec)
{
	__sync_fetch_and_sub(&v->cnt, dec);
}

/**
 * Atomically increment a counter by one.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
atomic16_inc(atomic16_t *v);

static inline void
atomic16_inc(atomic16_t *v)
{
	atomic16_add(v, 1);
}

/**
 * Atomically decrement a counter by one.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
atomic16_dec(atomic16_t *v);

static inline void
atomic16_dec(atomic16_t *v)
{
	atomic16_sub(v, 1);
}

/**
 * Atomically add a 16-bit value to a counter and return the result.
 *
 * Atomically adds the 16-bits value (inc) to the atomic counter (v) and
 * returns the value of v after addition.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 * @return
 *   The value of v after the addition.
 */
static inline int16_t
atomic16_add_return(atomic16_t *v, int16_t inc)
{
	return __sync_add_and_fetch(&v->cnt, inc);
}

/**
 * Atomically subtract a 16-bit value from a counter and return
 * the result.
 *
 * Atomically subtracts the 16-bit value (inc) from the atomic counter
 * (v) and returns the value of v after the subtraction.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 * @return
 *   The value of v after the subtraction.
 */
static inline int16_t
atomic16_sub_return(atomic16_t *v, int16_t dec)
{
	return __sync_sub_and_fetch(&v->cnt, dec);
}

/**
 * Atomically increment a 16-bit counter by one and test.
 *
 * Atomically increments the atomic counter (v) by one and returns true if
 * the result is 0, or false in all other cases.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   True if the result after the increment operation is 0; false otherwise.
 */
static inline int atomic16_inc_and_test(atomic16_t *v);

static inline int atomic16_inc_and_test(atomic16_t *v)
{
	return __sync_add_and_fetch(&v->cnt, 1) == 0;
}

/**
 * Atomically decrement a 16-bit counter by one and test.
 *
 * Atomically decrements the atomic counter (v) by one and returns true if
 * the result is 0, or false in all other cases.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   True if the result after the decrement operation is 0; false otherwise.
 */
static inline int atomic16_dec_and_test(atomic16_t *v);

static inline int atomic16_dec_and_test(atomic16_t *v)
{
	return __sync_sub_and_fetch(&v->cnt, 1) == 0;
}

/**
 * Atomically test and set a 16-bit atomic counter.
 *
 * If the counter value is already set, return 0 (failed). Otherwise, set
 * the counter value to 1 and return 1 (success).
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   0 if failed; else 1, success.
 */
static inline int atomic16_test_and_set(atomic16_t *v);

static inline int atomic16_test_and_set(atomic16_t *v)
{
	return atomic16_cmpset((volatile uint16_t *)&v->cnt, 0, 1);
}

/**
 * Atomically set a 16-bit counter to 0.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void atomic16_clear(atomic16_t *v)
{
	v->cnt = 0;
}

/*------------------------- 32 bit atomic operations -------------------------*/

/**
 * Atomic compare and set.
 *
 * (atomic) equivalent to:
 *   if (*dst == exp)
 *     *dst = src (all 32-bit words)
 *
 * @param dst
 *   The destination location into which the value will be written.
 * @param exp
 *   The expected value.
 * @param src
 *   The new value.
 * @return
 *   Non-zero on success; 0 on failure.
 */
static inline int
atomic32_cmpset(volatile uint32_t *dst, uint32_t exp, uint32_t src);

static inline int
atomic32_cmpset(volatile uint32_t *dst, uint32_t exp, uint32_t src)
{
	return __sync_bool_compare_and_swap(dst, exp, src);
}

/**
 * Atomic exchange.
 *
 * (atomic) equivalent to:
 *   ret = *dst
 *   *dst = val;
 *   return ret;
 *
 * @param dst
 *   The destination location into which the value will be written.
 * @param val
 *   The new value.
 * @return
 *   The original value at that location
 */
static inline uint32_t
atomic32_exchange(volatile uint32_t *dst, uint32_t val);

static inline uint32_t
atomic32_exchange(volatile uint32_t *dst, uint32_t val)
{
#if defined(__clang__)
	return __atomic_exchange_n(dst, val, __ATOMIC_SEQ_CST);
#else
	return __atomic_exchange_4(dst, val, __ATOMIC_SEQ_CST);
#endif
}

/**
 * The atomic counter structure.
 */
typedef struct {
	volatile int32_t cnt; /**< An internal counter value. */
} atomic32_t;

/**
 * Static initializer for an atomic counter.
 */
#define RTE_ATOMIC32_INIT(val) { (val) }

/**
 * Initialize an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
atomic32_init(atomic32_t *v)
{
	v->cnt = 0;
}

/**
 * Atomically read a 32-bit value from a counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   The value of the counter.
 */
static inline int32_t
atomic32_read(const atomic32_t *v)
{
	return v->cnt;
}

/**
 * Atomically set a counter to a 32-bit value.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param new_value
 *   The new value for the counter.
 */
static inline void
atomic32_set(atomic32_t *v, int32_t new_value)
{
	v->cnt = new_value;
}

/**
 * Atomically add a 32-bit value to an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 */
static inline void
atomic32_add(atomic32_t *v, int32_t inc)
{
	__sync_fetch_and_add(&v->cnt, inc);
}

/**
 * Atomically subtract a 32-bit value from an atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 */
static inline void
atomic32_sub(atomic32_t *v, int32_t dec)
{
	__sync_fetch_and_sub(&v->cnt, dec);
}

/**
 * Atomically increment a counter by one.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
atomic32_inc(atomic32_t *v);

static inline void
atomic32_inc(atomic32_t *v)
{
	atomic32_add(v, 1);
}

/**
 * Atomically decrement a counter by one.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
atomic32_dec(atomic32_t *v);

static inline void
atomic32_dec(atomic32_t *v)
{
	atomic32_sub(v,1);
}

/**
 * Atomically add a 32-bit value to a counter and return the result.
 *
 * Atomically adds the 32-bits value (inc) to the atomic counter (v) and
 * returns the value of v after addition.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 * @return
 *   The value of v after the addition.
 */
static inline int32_t
atomic32_add_return(atomic32_t *v, int32_t inc)
{
	return __sync_add_and_fetch(&v->cnt, inc);
}

/**
 * Atomically subtract a 32-bit value from a counter and return
 * the result.
 *
 * Atomically subtracts the 32-bit value (inc) from the atomic counter
 * (v) and returns the value of v after the subtraction.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 * @return
 *   The value of v after the subtraction.
 */
static inline int32_t
atomic32_sub_return(atomic32_t *v, int32_t dec)
{
	return __sync_sub_and_fetch(&v->cnt, dec);
}

/**
 * Atomically increment a 32-bit counter by one and test.
 *
 * Atomically increments the atomic counter (v) by one and returns true if
 * the result is 0, or false in all other cases.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   True if the result after the increment operation is 0; false otherwise.
 */
static inline int atomic32_inc_and_test(atomic32_t *v);

static inline int atomic32_inc_and_test(atomic32_t *v)
{
	return __sync_add_and_fetch(&v->cnt, 1) == 0;
}

/**
 * Atomically decrement a 32-bit counter by one and test.
 *
 * Atomically decrements the atomic counter (v) by one and returns true if
 * the result is 0, or false in all other cases.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   True if the result after the decrement operation is 0; false otherwise.
 */
static inline int atomic32_dec_and_test(atomic32_t *v);

static inline int atomic32_dec_and_test(atomic32_t *v)
{
	return __sync_sub_and_fetch(&v->cnt, 1) == 0;
}

/**
 * Atomically test and set a 32-bit atomic counter.
 *
 * If the counter value is already set, return 0 (failed). Otherwise, set
 * the counter value to 1 and return 1 (success).
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   0 if failed; else 1, success.
 */
static inline int atomic32_test_and_set(atomic32_t *v);

static inline int atomic32_test_and_set(atomic32_t *v)
{
	return atomic32_cmpset((volatile uint32_t *)&v->cnt, 0, 1);
}

/**
 * Atomically set a 32-bit counter to 0.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void atomic32_clear(atomic32_t *v)
{
	v->cnt = 0;
}

/*------------------------- 64 bit atomic operations -------------------------*/

/**
 * An atomic compare and set function used by the mutex functions.
 * (atomic) equivalent to:
 *   if (*dst == exp)
 *     *dst = src (all 64-bit words)
 *
 * @param dst
 *   The destination into which the value will be written.
 * @param exp
 *   The expected value.
 * @param src
 *   The new value.
 * @return
 *   Non-zero on success; 0 on failure.
 */
static inline int
atomic64_cmpset(volatile uint64_t *dst, uint64_t exp, uint64_t src);

static inline int
atomic64_cmpset(volatile uint64_t *dst, uint64_t exp, uint64_t src)
{
	return __sync_bool_compare_and_swap(dst, exp, src);
}

/**
 * Atomic exchange.
 *
 * (atomic) equivalent to:
 *   ret = *dst
 *   *dst = val;
 *   return ret;
 *
 * @param dst
 *   The destination location into which the value will be written.
 * @param val
 *   The new value.
 * @return
 *   The original value at that location
 */
static inline uint64_t
atomic64_exchange(volatile uint64_t *dst, uint64_t val);

static inline uint64_t
atomic64_exchange(volatile uint64_t *dst, uint64_t val)
{
#if defined(__clang__)
	return __atomic_exchange_n(dst, val, __ATOMIC_SEQ_CST);
#else
	return __atomic_exchange_8(dst, val, __ATOMIC_SEQ_CST);
#endif
}

/**
 * The atomic counter structure.
 */
typedef struct {
	volatile int64_t cnt;  /**< Internal counter value. */
} atomic64_t;

/**
 * Static initializer for an atomic counter.
 */
#define RTE_ATOMIC64_INIT(val) { (val) }

/**
 * Initialize the atomic counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
atomic64_init(atomic64_t *v);

static inline void
atomic64_init(atomic64_t *v)
{
	int success = 0;
	uint64_t tmp;

	while (success == 0) {
		tmp = v->cnt;
		success = atomic64_cmpset((volatile uint64_t *)&v->cnt,
		                              tmp, 0);
	}
}

/**
 * Atomically read a 64-bit counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   The value of the counter.
 */
static inline int64_t
atomic64_read(atomic64_t *v);

static inline int64_t
atomic64_read(atomic64_t *v)
{
	int success = 0;
	uint64_t tmp;

	while (success == 0) {
		tmp = v->cnt;
		/* replace the value by itself */
		success = atomic64_cmpset((volatile uint64_t *)&v->cnt,
		                              tmp, tmp);
	}
	return tmp;
}

/**
 * Atomically set a 64-bit counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param new_value
 *   The new value of the counter.
 */
static inline void
atomic64_set(atomic64_t *v, int64_t new_value);

static inline void
atomic64_set(atomic64_t *v, int64_t new_value)
{
	int success = 0;
	uint64_t tmp;

	while (success == 0) {
		tmp = v->cnt;
		success = atomic64_cmpset((volatile uint64_t *)&v->cnt,
		                              tmp, new_value);
	}
}

/**
 * Atomically add a 64-bit value to a counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 */
static inline void
atomic64_add(atomic64_t *v, int64_t inc);

static inline void
atomic64_add(atomic64_t *v, int64_t inc)
{
	__sync_fetch_and_add(&v->cnt, inc);
}

/**
 * Atomically subtract a 64-bit value from a counter.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 */
static inline void
atomic64_sub(atomic64_t *v, int64_t dec);

static inline void
atomic64_sub(atomic64_t *v, int64_t dec)
{
	__sync_fetch_and_sub(&v->cnt, dec);
}

/**
 * Atomically increment a 64-bit counter by one and test.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
atomic64_inc(atomic64_t *v);

static inline void
atomic64_inc(atomic64_t *v)
{
	atomic64_add(v, 1);
}

/**
 * Atomically decrement a 64-bit counter by one and test.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void
atomic64_dec(atomic64_t *v);

static inline void
atomic64_dec(atomic64_t *v)
{
	atomic64_sub(v, 1);
}

/**
 * Add a 64-bit value to an atomic counter and return the result.
 *
 * Atomically adds the 64-bit value (inc) to the atomic counter (v) and
 * returns the value of v after the addition.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param inc
 *   The value to be added to the counter.
 * @return
 *   The value of v after the addition.
 */
static inline int64_t
atomic64_add_return(atomic64_t *v, int64_t inc);

static inline int64_t
atomic64_add_return(atomic64_t *v, int64_t inc)
{
	return __sync_add_and_fetch(&v->cnt, inc);
}

/**
 * Subtract a 64-bit value from an atomic counter and return the result.
 *
 * Atomically subtracts the 64-bit value (dec) from the atomic counter (v)
 * and returns the value of v after the subtraction.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @param dec
 *   The value to be subtracted from the counter.
 * @return
 *   The value of v after the subtraction.
 */
static inline int64_t
atomic64_sub_return(atomic64_t *v, int64_t dec);

static inline int64_t
atomic64_sub_return(atomic64_t *v, int64_t dec)
{
	return __sync_sub_and_fetch(&v->cnt, dec);
}

/**
 * Atomically increment a 64-bit counter by one and test.
 *
 * Atomically increments the atomic counter (v) by one and returns
 * true if the result is 0, or false in all other cases.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   True if the result after the addition is 0; false otherwise.
 */
static inline int atomic64_inc_and_test(atomic64_t *v);

static inline int atomic64_inc_and_test(atomic64_t *v)
{
	return atomic64_add_return(v, 1) == 0;
}

/**
 * Atomically decrement a 64-bit counter by one and test.
 *
 * Atomically decrements the atomic counter (v) by one and returns true if
 * the result is 0, or false in all other cases.
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   True if the result after subtraction is 0; false otherwise.
 */
static inline int atomic64_dec_and_test(atomic64_t *v);

static inline int atomic64_dec_and_test(atomic64_t *v)
{
	return atomic64_sub_return(v, 1) == 0;
}

/**
 * Atomically test and set a 64-bit atomic counter.
 *
 * If the counter value is already set, return 0 (failed). Otherwise, set
 * the counter value to 1 and return 1 (success).
 *
 * @param v
 *   A pointer to the atomic counter.
 * @return
 *   0 if failed; else 1, success.
 */
static inline int atomic64_test_and_set(atomic64_t *v);

static inline int atomic64_test_and_set(atomic64_t *v)
{
	return atomic64_cmpset((volatile uint64_t *)&v->cnt, 0, 1);
}

/**
 * Atomically set a 64-bit counter to 0.
 *
 * @param v
 *   A pointer to the atomic counter.
 */
static inline void atomic64_clear(atomic64_t *v);

static inline void atomic64_clear(atomic64_t *v)
{
	atomic64_set(v, 0);
}

#endif /* _ATOMIC_H_ */

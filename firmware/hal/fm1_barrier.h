/* SPDX-License-Identifier: GPL-3.0-only */
#pragma once

/*
 * Compiler-only publication barrier for small shared-memory handoffs.
 *
 * Hardware-specific / compiler-specific primitives live under firmware/hal so
 * ordinary tracker modules do not embed inline assembly. This emits no CPU
 * instruction; it only prevents the compiler from reordering memory accesses
 * across the publication point.
 */
#if defined(__GNUC__) || defined(__clang__)
static inline void fm1_compiler_barrier(void)
{
    __asm__ __volatile__("" ::: "memory");
}
#else
static inline void fm1_compiler_barrier(void)
{
}
#endif

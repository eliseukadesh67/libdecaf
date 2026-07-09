/**
 * @cond internal
 * @file f_arithmetic.c
 * @copyright
 *   Copyright (c) 2014 Cryptography Research, Inc.  \n
 *   Released under the MIT License.  See LICENSE.txt for license information.
 * @author Mike Hamburg
 * @brief Field-specific arithmetic for E-521 (p = 2^521 - 1).
 *
 * Inverse-square-root addition chain ported from the historical decaf p521
 * backend and adapted to the current mask_t gf_isr() interface.  Because
 * p = 3 (mod 4), a = x^((p-3)/4) is the inverse square root and
 * a^2 * x == 1 exactly when x is a quadratic residue.
 */

#include "field.h"

mask_t gf_isr (
    gf a,
    const gf x
) {
    gf L0, L1, L2;
    gf_sqr  (   L1,     x );
    gf_mul  (   L0,     x,   L1 );
    gf_sqrn (   L2,   L0,     2 );
    gf_mul  (   L1,   L0,   L2 );
    gf_sqrn (   L2,   L1,     4 );
    gf_mul  (   L0,   L1,   L2 );
    gf_sqrn (   L2,   L0,     8 );
    gf_mul  (   L1,   L0,   L2 );
    gf_sqrn (   L2,   L1,    16 );
    gf_mul  (   L0,   L1,   L2 );
    gf_sqrn (   L2,   L0,    32 );
    gf_mul  (   L1,   L0,   L2 );
    gf_sqr  (   L2,   L1 );
    gf_mul  (   L0,     x,   L2 );
    gf_sqrn (   L2,   L0,    64 );
    gf_mul  (   L0,   L1,   L2 );
    gf_sqrn (   L2,   L0,   129 );
    gf_mul  (   L1,   L0,   L2 );
    gf_sqr  (   L2,   L1 );
    gf_mul  (   L0,     x,   L2 );
    gf_sqrn (   L2,   L0,   259 );
    gf_mul  (   L1,   L0,   L2 );
    gf_sqr  (   L0,   L1 );
    gf_mul  (     a,     x,   L0 ); /* a = x^((p-3)/4), candidate inverse sqrt */

    /* Verify: a^2 * x == 1 iff x is a quadratic residue. */
    gf_sqr  (   L1,     a );
    gf_mul  (   L2,   L1,   x );
    return gf_eq(L2, ONE);
}

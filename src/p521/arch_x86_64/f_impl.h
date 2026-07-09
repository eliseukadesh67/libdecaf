/* Copyright (c) 2014-2016 Cryptography Research, Inc.
 * Released under the MIT License.  See LICENSE.txt for license information.
 *
 * E-521 field backend (p = 2^521 - 1), portable 64-bit reference.
 * Ported from the historical decaf p521 implementation and adapted to the
 * current libdecaf per-field interface (9 limbs of 58/57 bits).
 */

#define GF_HEADROOM 9999 /* Everything is reduced anyway */
#define FIELD_LITERAL(a,b,c,d,e,f,g,h,i) {{a,b,c,d,e,f,g,h,i}}

/* p = 2^521-1 packed as 8 limbs of 58 bits + a top limb of 57 bits (8*58+57=521).
 * gf_strong_reduce / gf_serialize propagate carries using LIMB_PLACE_VALUE, so the
 * top limb MUST report 57, otherwise canonical reduction corrupts the high limb. */
#define LIMB_PLACE_VALUE(i) (((i)==8) ? 57 : 58)

void gf_add_RAW (gf out, const gf a, const gf b) {
    for (unsigned int i=0; i<9; i++) {
        out->limb[i] = a->limb[i] + b->limb[i];
    }
    gf_weak_reduce(out);
}

void gf_sub_RAW (gf out, const gf a, const gf b) {
    uint64_t co1 = ((1ull<<58)-1)*4, co2 = ((1ull<<57)-1)*4;
    for (unsigned int i=0; i<9; i++) {
        out->limb[i] = a->limb[i] - b->limb[i] + ((i==8) ? co2 : co1);
    }
    gf_weak_reduce(out);
}

void gf_bias (gf a, int amt) {
    (void) a;
    (void) amt;
}

void gf_weak_reduce (gf a) {
    uint64_t mask = (1ull<<58) - 1;
    uint64_t tmp = a->limb[8] >> 57;
    for (unsigned int i=8; i>0; i--) {
        a->limb[i] = (a->limb[i] & ((i==8) ? mask>>1 : mask)) + (a->limb[i-1]>>58);
    }
    a->limb[0] = (a->limb[0] & mask) + tmp;
}

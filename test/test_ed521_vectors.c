/* Validation of the libdecaf EdDSA E-521 implementation against a set of
 * official Ed521 test vectors.
 *
 * The vectors (in ed521_vectors.h) are taken from a previous undergraduate
 * thesis (TCC) by a student at the University of Brasilia (UnB): the reference
 * Ed521 implementation on which this work is based. They were transcribed from
 * that work's appendix of test vectors.
 *
 * For each vector this test checks: (1) the derived public key == expected;
 * (2) the generated signature == expected; (3) the reference signature verifies.
 *
 * Build: run ./run_e521_tests.sh from the libdecaf/ root, which builds the
 * library and compiles/runs this test. To build it by hand against an existing
 * build directory <BUILD> (e.g. build-e521):
 *   gcc -O2 test/test_ed521_vectors.c -Itest -I<BUILD>/src/GENERATED/include \
 *       <BUILD>/src/libdecaf.a -o test_ed521_vectors && ./test_ed521_vectors
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <decaf/ed521.h>
#include "ed521_vectors.h"

#define PUB DECAF_EDDSA_521_PUBLIC_BYTES
#define SIG DECAF_EDDSA_521_SIGNATURE_BYTES

static uint8_t* unhex(const char*h, size_t *outlen){
    size_t n=strlen(h)/2; uint8_t*o=malloc(n?n:1);
    for(size_t i=0;i<n;i++){ unsigned v; sscanf(h+2*i,"%2x",&v); o[i]=(uint8_t)v; }
    *outlen=n; return o;
}

int main(void){
    int fails=0;
    printf("Validation against the %d reference Ed521 test vectors (UnB TCC)\n", N_VEC);
    printf("+-----+----------+----------+-----------+----------+\n");
    printf("| vec | msg (B)  | pub.key  | signature | verify   |\n");
    printf("+-----+----------+----------+-----------+----------+\n");
    for(int k=0;k<N_VEC;k++){
        size_t pl,ul,ml,sl;
        uint8_t *priv=unhex(V_PRIV[k],&pl);
        uint8_t *epub=unhex(V_PUB[k],&ul);
        uint8_t *msg =unhex(V_MSG[k],&ml);
        uint8_t *esig=unhex(V_SIG[k],&sl);

        uint8_t pub[PUB], sig[SIG];
        decaf_ed521_derive_public_key(pub, priv);
        decaf_ed521_sign(sig, priv, pub, msg, ml, 0, NULL, 0);
        decaf_error_t v = decaf_ed521_verify(esig, epub, msg, ml, 0, NULL, 0);

        int pub_ok = (memcmp(pub,epub,PUB)==0);
        int sig_ok = (memcmp(sig,esig,SIG)==0);
        int ver_ok = (v==DECAF_SUCCESS);
        fails += !(pub_ok && sig_ok && ver_ok);
        printf("| %3d | %8d | %-8s | %-9s | %-8s |\n", k, (int)ml,
               pub_ok?"OK":"FAIL", sig_ok?"OK":"FAIL", ver_ok?"VALID":"FAIL");
        free(priv);free(epub);free(msg);free(esig);
    }
    printf("+-----+----------+----------+-----------+----------+\n");
    printf("\nRESULT: %s\n", fails==0 ?
        "ALL REFERENCE VECTORS MATCH (full interop)" : "THERE WERE FAILURES");
    return fails?1:0;
}

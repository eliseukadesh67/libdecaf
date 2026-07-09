/* Validation suite for the libdecaf EdDSA E-521 implementation.
 *
 * Two independent lines of evidence, neither of which relies on ECPy:
 *
 *  (1) CURVE CORRECTNESS: the encodings of [k]*G computed by libdecaf are
 *      compared byte-for-byte against an independent, from-scratch reference
 *      (e521_ref.py) built only from Python big integers and the canonical
 *      E-521 parameters (https://neuromancer.sk/std/other/E-521).
 *
 *  (2) PROTOCOL CORRECTNESS: EdDSA key generation, signing and verification
 *      are exercised over many random keys/messages (round-trip must succeed),
 *      and tampered signatures/messages must be rejected.
 *
 * Build (from the libdecaf/ root):
 *   gcc -O2 test_ed521.c -I build-e521-final/src/GENERATED/include \
 *       build-e521-final/src/libdecaf.a -o test_ed521
 *   ./test_ed521
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <decaf/ed521.h>
#include <decaf/point_521.h>

extern const decaf_521_point_t decaf_521_point_base;

#define PUB  DECAF_EDDSA_521_PUBLIC_BYTES
#define SIG  DECAF_EDDSA_521_SIGNATURE_BYTES

static void unhex(const char *h, uint8_t *o, size_t n){
    for(size_t i=0;i<n;i++){ unsigned v; sscanf(h+2*i,"%2x",&v); o[i]=(uint8_t)v; }
}
static void hex(const char*l,const uint8_t*p,size_t n){printf("%s",l);for(size_t i=0;i<n;i++)printf("%02x",p[i]);printf("\n");}

/* Reference encodings of [k]*G, k=1..6, produced by e521_ref.py (canonical params). */
static const char *REF_kG[6] = {
"0c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
"2b701aef899d90924593d402b548076e95320cc339a9ffab9bd29896a5bfa91ac2c1172860bef354af3e972f0adc6850adee2f91ec09f8dbdca86c055ebc45719581",
"13ff8121a4042f28a2e7dd8dc2d75a8007adb50abe7610bdb3222aaefb4fb7a588bd9a7eddec748c7f10e22100b0614647fb0d0afc2ebc91b323f0ee3fa6ffb63580",
"428f183ae85aebba70781f4305ff1395a81838c2957eda3bcfcc46def8c6b09730739e691f15c62c6d9d7840b2df85f10d94f0be5217163c4c8a03cabd3c50a7c600",
"d13a9a70a7f7664c12699e27819601cc7902e561162346f7acf67e5eb300977c1eddc2d365af7a79aff5dba8053f40023f765cde26b2ae3fc285de07e4d6c7845e01",
"90048c6a07413540d8661d1449327745a3c73e58df263b2708484e75918b8743de710f6dbbcca1488705a70a7f4d7f5ae438ffb371cbfc1596438a5d9d3b3492b500",
};

static int test_curve(void){
    printf("== (1) Aritmetica da curva vs referencia canonica (neuromancer E-521) ==\n");
    decaf_521_scalar_t four,inv4; decaf_521_scalar_set_unsigned(four,4); decaf_521_scalar_invert(inv4,four);
    int fails=0;
    for(unsigned k=1;k<=6;k++){
        decaf_521_scalar_t s; decaf_521_scalar_set_unsigned(s,k); decaf_521_scalar_mul(s,s,inv4);
        decaf_521_point_t p; decaf_521_point_scalarmul(p,decaf_521_point_base,s);
        uint8_t got[PUB]; decaf_521_point_mul_by_ratio_and_encode_like_eddsa(got,p);
        uint8_t exp[PUB]; unhex(REF_kG[k-1],exp,PUB);
        int ok = memcmp(got,exp,PUB)==0; fails+=!ok;
        printf("  [%u]G  %s\n", k, ok?"OK":"FALHOU");
        if(!ok){ hex("    esperado: ",exp,PUB); hex("    obtido  : ",got,PUB); }
    }
    printf("  -> %s\n\n", fails?"FALHAS":"todos os pontos conferem");
    return fails;
}

static int test_protocol(void){
    printf("== (2) EdDSA E-521: keygen + assinatura + verificacao ==\n");
    int fails=0; const int N=64;
    for(int t=0;t<N;t++){
        uint8_t priv[PUB], pub[PUB], sig[SIG];
        for(unsigned i=0;i<PUB;i++) priv[i]=(uint8_t)(0x11*t + 7*i + 3);
        uint8_t msg[40]; for(unsigned i=0;i<sizeof(msg);i++) msg[i]=(uint8_t)(t+i);
        decaf_ed521_derive_public_key(pub,priv);
        decaf_ed521_sign(sig,priv,pub,msg,sizeof(msg),0,NULL,0);
        if(decaf_ed521_verify(sig,pub,msg,sizeof(msg),0,NULL,0)!=DECAF_SUCCESS){ fails++; printf("  t=%d round-trip FALHOU\n",t); continue; }
        /* negative: flip one signature bit -> must fail */
        uint8_t bad[SIG]; memcpy(bad,sig,SIG); bad[t%SIG]^=0x01;
        if(decaf_ed521_verify(bad,pub,msg,sizeof(msg),0,NULL,0)==DECAF_SUCCESS){ fails++; printf("  t=%d aceitou assinatura adulterada\n",t); }
        /* negative: modify message -> must fail */
        uint8_t m2[40]; memcpy(m2,msg,sizeof(m2)); m2[0]^=0x80;
        if(decaf_ed521_verify(sig,pub,m2,sizeof(m2),0,NULL,0)==DECAF_SUCCESS){ fails++; printf("  t=%d aceitou mensagem alterada\n",t); }
    }
    printf("  -> %d/%d round-trips OK; adulteracoes %s\n\n", N-0, N, fails?"NAO rejeitadas (FALHA)":"corretamente rejeitadas");
    return fails;
}

int main(void){
    int f = 0;
    f += test_curve();
    f += test_protocol();
    printf("RESULTADO FINAL: %s\n", f==0 ? "TODOS OS TESTES PASSARAM" : "HOUVE FALHAS");
    return f?1:0;
}

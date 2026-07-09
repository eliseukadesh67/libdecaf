/* Validacao da libdecaf EdDSA E-521 contra os vetores de teste oficiais do
 * Apendice C de Antunes (2021, UnB) -- a implementacao de referencia (Ed521/ECPy
 * em Dart) na qual este trabalho se baseia.
 *
 * Para cada vetor confere: (1) chave publica derivada == esperada;
 * (2) assinatura gerada == esperada; (3) a assinatura de referencia verifica.
 *
 * Build (a partir de libdecaf/):
 *   gcc -O2 test_penido.c -I build-e521-final/src/GENERATED/include \
 *       build-e521-final/src/libdecaf.a -o test_penido && ./test_penido
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <decaf/ed521.h>
#include "penido_vectors.h"

#define PUB DECAF_EDDSA_521_PUBLIC_BYTES
#define SIG DECAF_EDDSA_521_SIGNATURE_BYTES

static uint8_t* unhex(const char*h, size_t *outlen){
    size_t n=strlen(h)/2; uint8_t*o=malloc(n?n:1);
    for(size_t i=0;i<n;i++){ unsigned v; sscanf(h+2*i,"%2x",&v); o[i]=(uint8_t)v; }
    *outlen=n; return o;
}

int main(void){
    int fails=0;
    printf("Validacao contra os %d vetores do Apendice C (Antunes, 2021)\n", N_VEC);
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
               pub_ok?"OK":"FALHOU", sig_ok?"OK":"FALHOU", ver_ok?"VALIDA":"FALHOU");
        free(priv);free(epub);free(msg);free(esig);
    }
    printf("+-----+----------+----------+-----------+----------+\n");
    printf("\nRESULTADO: %s\n", fails==0 ? "TODOS OS 8 VETORES DO PENIDO CONFEREM (interop total)" : "HOUVE FALHAS");
    return fails?1:0;
}

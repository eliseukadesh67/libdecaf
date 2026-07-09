/* Testador manual do EdDSA E-521.
 *
 * Uso:
 *   ./meu_teste "sua mensagem aqui"                 (chave privada padrao)
 *   ./meu_teste "sua mensagem"  <chave_priv_hex>    (132 caracteres hex = 66 bytes)
 *
 * Compilar (a partir de libdecaf/):
 *   gcc -O2 meu_teste.c -I build-e521-final/src/GENERATED/include \
 *       build-e521-final/src/libdecaf.a -o meu_teste
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <decaf/ed521.h>

#define PUB  DECAF_EDDSA_521_PUBLIC_BYTES     /* 66  */
#define SIG  DECAF_EDDSA_521_SIGNATURE_BYTES  /* 132 */

static void printhex(const char *label, const uint8_t *p, size_t n){
    printf("%-16s", label);
    for (size_t i=0;i<n;i++) printf("%02x", p[i]);
    printf("\n");
}

int main(int argc, char **argv){
    const char *msg = (argc >= 2) ? argv[1] : "Mensagem de teste E-521";

    /* Chave privada: 66 bytes. Padrao = um valor fixo; pode passar outra em hex. */
    uint8_t priv[PUB];
    for (unsigned i=0;i<PUB;i++) priv[i] = (uint8_t)(0x42 + i);   /* semente padrao */
    if (argc >= 3) {
        if (strlen(argv[2]) != 2*PUB) {
            fprintf(stderr, "chave privada deve ter %d bytes (%d chars hex)\n", PUB, 2*PUB);
            return 2;
        }
        for (unsigned i=0;i<PUB;i++){ unsigned v; sscanf(argv[2]+2*i,"%2x",&v); priv[i]=(uint8_t)v; }
    }

    uint8_t pub[PUB], sig[SIG];

    /* 1) Deriva a chave publica */
    decaf_ed521_derive_public_key(pub, priv);

    /* 2) Assina a mensagem */
    decaf_ed521_sign(sig, priv, pub, (const uint8_t*)msg, strlen(msg), 0, NULL, 0);

    /* 3) Verifica */
    decaf_error_t ok = decaf_ed521_verify(sig, pub, (const uint8_t*)msg, strlen(msg), 0, NULL, 0);

    /* 4) Teste negativo: altera 1 bit da assinatura -> deve falhar */
    uint8_t sig_ruim[SIG]; memcpy(sig_ruim, sig, SIG); sig_ruim[0] ^= 0x01;
    decaf_error_t ruim = decaf_ed521_verify(sig_ruim, pub, (const uint8_t*)msg, strlen(msg), 0, NULL, 0);

    printf("Mensagem   : \"%s\"  (%zu bytes)\n", msg, strlen(msg));
    printhex("Chave priv :", priv, PUB);
    printhex("Chave pub  :", pub, PUB);
    printhex("Assinatura :", sig, SIG);
    printf("\n");
    printf("Verificacao (assinatura correta) : %s\n", ok==DECAF_SUCCESS ? "VALIDA  (OK)" : "INVALIDA (ERRO!)");
    printf("Verificacao (assinatura adulterada): %s\n", ruim==DECAF_SUCCESS ? "aceita (ERRO!)" : "rejeitada (OK)");
    return (ok==DECAF_SUCCESS && ruim!=DECAF_SUCCESS) ? 0 : 1;
}

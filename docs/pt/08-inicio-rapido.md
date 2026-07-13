# Início rápido (C)

Exemplo mínimo de geração de par de chaves X25519:

```c
#include <decaf.h>
#include <decaf/ed255.h>

uint8_t privkey[32], pubkey[32];
decaf_x25519_generate_key(privkey);
decaf_x25519_derive_public_key(pubkey, privkey);
```

Compile ligando `-ldecaf` e apontando o include para `include/decaf/` (instalado ou em `src/GENERATED/include/`).

Para EdDSA, inclua `decaf/ed255.h` ou `decaf/ed448.h` e use `decaf_ed25519_sign` / `decaf_ed448_sign` (consulte os cabeçalhos gerados para tamanhos de buffer).

Exemplo após instalação:

```bash
gcc -o x25519_demo demo.c -ldecaf -I/usr/local/include
```

Fluxo típico no TCC (análise do repositório):

```bash
cd /caminho/para/libdecaf
cmake -B build -DCMAKE_INSTALL_PREFIX=./install
cmake --build build
cd build && ctest
```

## Compilando e validando o EdDSA E-521

Para a curva E-521, o caminho mais rápido é o script de um comando na raiz do
repositório. A partir de `libdecaf/` (requer `cmake`, `make` e um compilador C):

```bash
./run_e521_tests.sh
```

Ele compila a biblioteca com o alvo E-521, gera as tabelas do ponto base e então
compila e executa as duas suítes de validação (`test/test_ed521_vectors.c` e
`test/test_ed521.c`). A API pública do EdDSA é `decaf/ed521.h` (chaves de 66 bytes,
assinaturas de 132 bytes, SHAKE256). Para compilar a biblioteca manualmente, veja
a [seção "Evaluating the E-521 implementation" do README raiz](../../README.md) ou
[Compilação e geração de código](04-compilacao-e-geracao.md).

Documentação da API gerada por Doxygen: `make doc` no diretório de build (requer Doxygen instalado).

[← Segurança e testes](07-seguranca-e-testes.md) · [Índice](README.md)

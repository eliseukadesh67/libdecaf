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

Documentação da API gerada por Doxygen: `make doc` no diretório de build (requer Doxygen instalado).

[← Segurança e testes](07-seguranca-e-testes.md) · [Índice](README.md)

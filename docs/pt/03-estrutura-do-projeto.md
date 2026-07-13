# Estrutura do projeto

```
libdecaf/
├── CMakeLists.txt          # Compilação principal (CMake 3.0+)
├── Makefile                # Compilação alternativa (GNU Make)
├── Doxyfile.in             # Documentação da API (make doc)
├── README.md               # Visão geral upstream
├── HISTORY.txt             # Notas de versão
├── TODO.txt                # Pendências dos mantenedores
├── LICENSE.txt
│
├── docs/                   # Esta documentação (pt/ e en/)
├── cmake/                  # Configuração CMake para instalação
├── python/                 # Wrapper opcional (edgold)
│   └── edgold/
│
├── src/
│   ├── CMakeLists.txt      # Alvo libdecaf, seleção de arquitetura
│   ├── utils.c, shake.c, sha512.c, spongerng.c
│   │
│   ├── public_include/     # Cabeçalhos públicos e templates
│   │   └── decaf/
│   ├── include/            # Cabeçalhos internos, intrínsecos por arch
│   │   ├── constant_time.h, field.h, word.h
│   │   └── arch_{32,ref64,x86_64,arm_32,neon}/
│   │
│   ├── per_field/          # Templates de corpo finito
│   ├── per_curve/          # Templates de curva (decaf, scalar, eddsa, elligator)
│   │
│   ├── p25519/             # Campo Curve25519 + f_impl por arquitetura
│   ├── p448/               # Campo Ed448 + f_impl por arquitetura
│   ├── curve25519/         # Objeto da curva Ristretto + decaf_tables.c
│   ├── ed448goldilocks/    # Objeto Ed448 + decaf_tables.c
│   │
│   ├── generator/          # Geração Python + alvos CMake
│   │   ├── template.py
│   │   └── curve_data.py
│   │
│   └── GENERATED/          # Saídas pré-geradas (também no diretório de build)
│       ├── include/decaf/
│       └── c/{curve25519,ed448goldilocks,p25519,p448}/
│
├── test/                   # Testes CTest + scripts Sage
└── _aux/                   # Scripts de pesquisa — fora da biblioteca
```

## Cabeçalhos gerados principais (após compilar)

| Cabeçalho | Função |
|-----------|--------|
| `decaf.h` | Inclusão mestra em C (ambas as curvas) |
| `decaf.hxx` | Inclusão mestra em C++ |
| `decaf/point_255.h` | API do grupo Ristretto (Curve25519) |
| `decaf/point_448.h` | API do grupo Ed448 |
| `decaf/ed255.h` | Ed25519 / EdDSA |
| `decaf/ed448.h` | Ed448 / EdDSA |
| `decaf/sha512.h`, `decaf/shake.h` | Funções de hash |

Padrão de prefixos: **`decaf_255_*`** para a curva de 255 bits; **`decaf_448_*`** para Ed448.

[← Arquitetura](02-arquitetura.md) · [Próximo: Compilação e geração →](04-compilacao-e-geracao.md)

# API pública

## Estrutura da API em C

```
decaf.h
├── decaf/point_255.h    # Grupo: decaf_255_point_t, decaf_255_scalar_t
├── decaf/point_448.h    # Grupo: decaf_448_point_t, decaf_448_scalar_t
├── decaf/ed255.h        # EdDSA + decaf_x25519_* (RFC 7748)
└── decaf/ed448.h        # EdDSA + decaf_x448_*
```

## Operações de grupo

Símbolos representativos: `decaf_255_*` / `decaf_448_*`.

| Categoria | Funções |
|-----------|---------|
| **Escalares** | `scalar_decode_long`, `scalar_encode`, `add`, `sub`, `mul`, `halve`, `div`, `eq`, `cond_sel`, `destroy` |
| **Pontos** | `decode`, `encode`, `add`, `sub`, `double`, `negate`, `eq`, `scalarmul`, `cond_sel`, `destroy` |
| **Mult. avançada** | `precompute`, `precomputed_scalarmul`, `point_double_scalarmul`, `point_dual_scalarmul`, `base_double_scalarmul_non_secret` |
| **Hash-para-curva** | `point_from_hash`, `point_from_hash_uniform`, inversos Elligator |
| **X25519/X448** | `x25519_generate_key`, `x25519_derive_public_key`, `point_mul_by_ratio_and_encode_like_x25519` (e análogos para 448) |

Constantes: `point_identity`, `point_base`, `precomputed_base`, `scalar_zero`, `scalar_one`.

## EdDSA (RFC 8032)

| Curva | Cabeçalho | Funções principais |
|-------|-----------|-------------------|
| Ed25519 | `decaf/ed255.h` | `decaf_ed25519_derive_public_key`, `sign`, `verify`, variantes com pré-hash |
| Ed448 | `decaf/ed448.h` | equivalentes `decaf_ed448_*` |

Ed25519 usa pré-hash **SHA-512**; Ed448 usa **SHAKE**. Strings de contexto e separação de domínio (`SigEd25519 no Ed25519 collisions`, `SigEd448`) estão embutidas no código gerado.

## API em C++

- Cabeçalhos: `decaf.hxx`, `decaf/point_255.hxx`, `decaf/ed255.hxx`, etc.
- **Somente cabeçalhos**: chamadas *inline* à API C
- **RAII** via `secure_buffer.hxx` para zerar buffers sensíveis ao sair de escopo
- Auxiliar `decaf::run_for_all_curves<Run>()` para código genérico sobre ambas as curvas

## Utilitários de hash

| Módulo | Papel |
|--------|--------|
| `decaf/sha512.h` | Ed25519, hash de 512 bits |
| `decaf/shake.h` | família SHAKE / SHA-3 |
| `decaf/spongerng.h` | interface de RNG tipo esponja |

Assinaturas exatas e tamanhos de buffer: ver `src/GENERATED/include/decaf/` ou executar `make doc` (Doxygen).

[← Compilação e geração](04-compilacao-e-geracao.md) · [Próximo: Criptografia →](06-criptografia.md)

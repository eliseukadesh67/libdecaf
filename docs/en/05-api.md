# Public API

## C API structure

```
decaf.h
├── decaf/point_255.h    # Group: decaf_255_point_t, decaf_255_scalar_t
├── decaf/point_448.h    # Group: decaf_448_point_t, decaf_448_scalar_t
├── decaf/ed255.h        # EdDSA + decaf_x25519_* (RFC 7748)
└── decaf/ed448.h        # EdDSA + decaf_x448_*
```

## Group operations

Representative symbols: `decaf_255_*` / `decaf_448_*`.

| Category | Functions |
|----------|-----------|
| **Scalars** | `scalar_decode_long`, `scalar_encode`, `add`, `sub`, `mul`, `halve`, `div`, `eq`, `cond_sel`, `destroy` |
| **Points** | `decode`, `encode`, `add`, `sub`, `double`, `negate`, `eq`, `scalarmul`, `cond_sel`, `destroy` |
| **Advanced mul** | `precompute`, `precomputed_scalarmul`, `point_double_scalarmul`, `point_dual_scalarmul`, `base_double_scalarmul_non_secret` |
| **Hash-to-curve** | `point_from_hash`, `point_from_hash_uniform`, Elligator inverse helpers |
| **X25519/X448** | `x25519_generate_key`, `x25519_derive_public_key`, `point_mul_by_ratio_and_encode_like_x25519` (and 448 analogs) |

Constants: `point_identity`, `point_base`, `precomputed_base`, `scalar_zero`, `scalar_one`.

## EdDSA (RFC 8032)

| Curve | Header | Key functions |
|-------|--------|---------------|
| Ed25519 | `decaf/ed255.h` | `decaf_ed25519_derive_public_key`, `sign`, `verify`, prehash variants |
| Ed448 | `decaf/ed448.h` | `decaf_ed448_*` equivalents |

Ed25519 uses **SHA-512** prehash; Ed448 uses **SHAKE**. Context strings and domain separation (`SigEd25519 no Ed25519 collisions`, `SigEd448`) are baked into generated code.

## C++ API

- Headers: `decaf.hxx`, `decaf/point_255.hxx`, `decaf/ed255.hxx`, etc.
- **Header-only**: inline wrappers over C calls
- **RAII** via `secure_buffer.hxx` to zero sensitive buffers on scope exit
- Template helper `decaf::run_for_all_curves<Run>()` for generic code over both curves

## Hash utilities

| Module | Role |
|--------|------|
| `decaf/sha512.h` | Ed25519, general 512-bit hash |
| `decaf/shake.h` | SHAKE / SHA-3 family |
| `decaf/spongerng.h` | Sponge-based RNG interface |

For exact signatures and buffer sizes, see generated headers under `src/GENERATED/include/decaf/` or run `make doc` (Doxygen).

[← Build & codegen](04-build-and-codegen.md) · [Next: Cryptography →](06-cryptography.md)

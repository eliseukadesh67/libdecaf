# Security and testing

## Constant-time discipline

Documented intent (see README and generated headers):

- Secret data must not control **array indices** or branches (except final success/failure checks).
- Implementation in `constant_time.h` — vector-friendly `constant_time_lookup`, `cond_swap`, etc.
- **Exception:** `decaf_*_base_double_scalarmul_non_secret` — explicitly not constant-time (public scalars only).

## Memory hygiene

- C++ `secure_buffer` clears on destruction
- `scalar_destroy` / `point_destroy` zeroize structures

## Caveats

- **No warranty**; treat as security-sensitive code requiring audit
- Timing attack mitigations attempted; **power analysis not** addressed
- RNG failure handling flagged as TODO in maintainer list

## Testing

### CMake / CTest (`test/`)

| Target | Purpose |
|--------|---------|
| `decaf_tester` | Main functional tests (`test_decaf.cxx`) |
| `ristretto_tester` | Ristretto-specific vectors (`ristretto.cxx`) |
| `shakesum_tester` | SHAKE test vectors |
| `bench` | Performance benchmarks |

Run: `make test` or `ctest` from build directory.

### Additional checks

| Command | Description |
|---------|-------------|
| `make test_ct` | Valgrind run of constant-time test (`test_ct.cxx`) |
| `make sagetest` | Sage-based algebraic tests (`test_decaf.sage`) |
| `make microbench` | Micro-benchmarks |

Vector includes: `test/vectors.inc.cxx`.

## Python bindings

Package **`edgold`** under `python/`:

- **ctypes** wrapper around `libdecaf.so`
- Primary module: `python/edgold/ed448.py` (Ed448-Goldilocks)
- Build: `python/setup.py` invokes `gmake lib` from parent directory
- Status: alpha

Not required for C/C++ consumers; optional scripting aid.

## Licensing and history

- **License:** MIT — Copyright 2014–2017 Cryptography Research, Inc. (Rambus) for core code; separate BSD terms on some Python files
- **Major milestones** ([HISTORY.txt](../../HISTORY.txt)):
  - **1.0** (July 2018): Johan Pascal CMake build integration
  - **Ristretto** encoding harmonization (Oct 2017)
  - Multi-curve Python template system (2016)
  - STROBE removed pre-1.0 (separate project)

## References

| Topic | Link |
|-------|------|
| Decaf cofactor technique | https://eprint.iacr.org/2015/673 |
| Ristretto encoding | https://ristretto.group |
| RFC 7748 (X25519/X448) | https://www.rfc-editor.org/rfc/rfc7748 |
| RFC 8032 (EdDSA) | https://www.rfc-editor.org/rfc/rfc8032 |
| Upstream README | [README.md](../../README.md) |

[← Cryptography](06-cryptography.md) · [Next: Quick start →](08-quickstart.md)

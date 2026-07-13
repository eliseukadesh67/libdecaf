# Cryptographic design

## Decaf / Ristretto cofactor elimination

Twisted Edwards curves used here have cofactor **4** (Ed448) or **8** (Curve25519). The library:

1. Works in the **even subgroup** of the Edwards curve (removes one factor of 2).
2. Quotients out remaining **2- or 4-torsion** so wire encodings represent cosets, not raw curve points.

Applications see a **prime-order group** of size ≈ p, which simplifies protocols and removes cofactor pitfalls.

## Curve25519 vs Ristretto naming

- Internal curve: **IsoEd25519** (isogenous to Ed25519 / Curve25519).
- Wire encoding: **Ristretto** (aligned with [ristretto.group](https://ristretto.group); breaking change from older Decaf encoding in v0.9.4 — see [HISTORY.txt](../../HISTORY.txt)).
- C++ class renamed to **Ristretto**; `IsoEd25519` kept as synonym.

## Coordinates and formulas

- **Extended twisted Edwards** coordinates `(x, y, z, t)` with `t = xy/z`.
- Formulas aim to be **complete** (no special cases for addition).
- Decoding can still **fail** (invalid encodings); signing/verification paths check return values.

## Elligator

`elligator.c` (per curve) implements hash-to-curve and inverse maps for protocols needing deterministic points from hashes (PAKE, VRF, steganographic encodings).

## Scalar multiplication strategy

Configured per curve in `curve_data.py`:

- **Comb method** (`combs`: n, t, s parameters)
- **wNAF** fixed/var table sizes
- **Window bits** for constant-time table lookups
- Precomputed **base point table** in `decaf_tables.c`

## Platform and performance

Each prime field has:

- `f_arithmetic.c` — core mod-p operations
- `f_impl.c` — selected per CPU (`arch_x86_64`, `arch_neon`, etc.)
- `f_generic.c` — generated portable fallbacks

Most platform-specific code volume lives in **`f_impl.c`** (hundreds of lines per arch, especially NEON).

### Known limitations (upstream)

- ARM NEON path historically ~15–20% slower than earlier standalone Goldilocks; x86_64 performance roughly unchanged
- Montgomery ladder removed (may affect some X25519-style code paths)
- Formal verification and exhaustive corner-case testing still open ([TODO.txt](../../TODO.txt))

[← Public API](05-api.md) · [Next: Security & testing →](07-security-and-testing.md)

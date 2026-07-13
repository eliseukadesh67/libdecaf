# Overview

**libdecaf** is a C/C++ library for research and production use of twisted Edwards curves. It exposes a **prime-order group** abstraction (via the Decaf/Ristretto technique), standard CFRG protocols, and low-level curve arithmetic.

| Attribute | Value |
|-----------|--------|
| **Project name** | DECAF (CMake) / libdecaf |
| **Version** | 1.0 (see [HISTORY.txt](../../HISTORY.txt)) |
| **Primary author** | Mike Hamburg (Cryptography Research / Rambus) |
| **License** | MIT ([LICENSE.txt](../../LICENSE.txt)) |
| **Language** | C99 library + C++11 header-only wrappers |
| **Curves** | Curve25519 / Ristretto (Ed25519 family), Ed448-Goldilocks |

The library is intended for developers who already understand elliptic-curve cryptography. It does **not** teach secure protocol design.

## Goals and supported standards

### Primary goals

- **X25519 / X448** — RFC 7748 elliptic-curve Diffie–Hellman key exchange
- **Ed25519 / EdDSA-Ed448** — RFC 8032 digital signatures
- **General EC toolkit** — points, scalars, precomputation, Elligator hashing, advanced protocol building blocks (PAKE, VRF, etc.)

### What it deliberately does *not* do

- Expose cofactor information (cofactor 4 or 8 is removed at the API level)
- Implement a full symmetric-crypto suite (only hashes required by EdDSA/X25519: SHA-512, SHA-3, SHAKE)
- Guarantee resistance to side channels beyond timing (e.g. no power-analysis hardening)

[Next: Architecture →](02-architecture.md)

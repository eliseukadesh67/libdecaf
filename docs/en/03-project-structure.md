# Project structure

```
libdecaf/
├── CMakeLists.txt          # Primary build (CMake 3.0+)
├── Makefile                # Alternative GNU Make build
├── Doxyfile.in             # API docs (make doc / cmake doc)
├── README.md               # Upstream overview
├── HISTORY.txt             # Release notes
├── TODO.txt                # Maintainer backlog
├── LICENSE.txt
│
├── docs/                   # This documentation
├── cmake/                  # Installed CMake package config
├── python/                 # Optional ctypes wrapper (edgold)
│   └── edgold/
│
├── src/
│   ├── CMakeLists.txt      # libdecaf target, arch selection
│   ├── utils.c, shake.c, sha512.c, spongerng.c
│   │
│   ├── public_include/     # Hand-written + template public headers
│   │   └── decaf/
│   ├── include/            # Internal headers, arch intrinsics
│   │   ├── constant_time.h, field.h, word.h
│   │   └── arch_{32,ref64,x86_64,arm_32,neon}/
│   │
│   ├── per_field/          # Field templates (f_generic, f_field)
│   ├── per_curve/          # Curve templates (decaf, scalar, eddsa, elligator)
│   │
│   ├── p25519/             # Curve25519 field + arch f_impl
│   ├── p448/               # Ed448 field + arch f_impl
│   ├── curve25519/         # Ristretto curve object + decaf_tables.c
│   ├── ed448goldilocks/    # Ed448 curve object + decaf_tables.c
│   │
│   ├── generator/          # Python codegen + CMake custom targets
│   │   ├── template.py
│   │   └── curve_data.py
│   │
│   └── GENERATED/          # Pre-generated outputs (also produced in build dir)
│       ├── include/decaf/
│       └── c/{curve25519,ed448goldilocks,p25519,p448}/
│
├── test/                   # CTest executables + Sage scripts
└── _aux/                   # Research scripts — not part of the library
```

## Key generated headers (after build)

| Header | Purpose |
|--------|---------|
| `decaf.h` | Master C include: pulls in both curves |
| `decaf.hxx` | Master C++ include |
| `decaf/point_255.h` | Ristretto group API (Curve25519) |
| `decaf/point_448.h` | Prime-order group API (Ed448) |
| `decaf/ed255.h` | Ed25519 / EdDSA |
| `decaf/ed448.h` | Ed448 / EdDSA |
| `decaf/sha512.h`, `decaf/shake.h` | Hash primitives |

Namespace prefix pattern: **`decaf_255_*`** for the 255-bit curve, **`decaf_448_*`** for Ed448.

[← Architecture](02-architecture.md) · [Next: Build & codegen →](04-build-and-codegen.md)

# Build system and code generation

Two equivalent entry points exist: **CMake** (recommended for install/packaging) and **Makefile** (used by the Python wrapper and some developers).

## CMake

```bash
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install /path/to/libdecaf
make
make test          # CTest: decaf_tester, bench, etc.
make install
make doc           # Doxygen, requires generatedCode
```

### CMake options

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_SHARED` | ON | Build `libdecaf.so` |
| `ENABLE_STATIC` | ON | Build `libdecaf.a` |
| `ENABLE_STRICT` | YES | `-Werror` and extra warnings |
| `ENABLE_TESTS` | YES | Build and register tests |
| `GENERATED_SOURCE_PATH` | OFF | Custom path for generated sources (default: `${BUILD_DIR}/src/GENERATED`) |

### Architecture auto-selection

| `CMAKE_SYSTEM_PROCESSOR` | Global intrinsics | p25519 | p448 |
|--------------------------|-------------------|--------|------|
| `x86_64` | `arch_x86_64` | `arch_x86_64` | `arch_x86_64` |
| `aarch64` / `arch64` | `arch_ref64` | `arch_ref64` | `arch_ref64` |
| `arm` (+ NEON on Android) | `arch_neon` or `arch_arm_32` | `arch_32` | `arch_neon` / `arch_arm_32` |
| default | `arch_32` | `arch_32` | `arch_32` |

Regenerate only precomputed tables:

```bash
make decaf_tables
```

## GNU Make

```bash
make              # libdecaf.so + internal test binaries
make test
make bench
make doc
make clean
```

Outputs land under `build/lib`, `build/bin`, with generated code mirrored to `src/GENERATED/`.

## Library artifact

- **Target names:** `decaf` (shared), `decaf-static` (static)
- **Object libraries:** `p25519`, `p448`, `CURVE25519`, `CURVE448` linked into one `libdecaf`
- **Install:** headers to `include/decaf/`, CMake config to `share/decaf/cmake`

## Code generation

### `curve_data.py`

Central parameter file for code generation.

**Fields:**

| Key | Modulus | Used by |
|-----|---------|---------|
| `p25519` | 2^255 − 19 | curve25519 |
| `p448` | 2^448 − 2^224 − 1 | ed448goldilocks |

**Curves:**

| Key | Display name | Cofactor removed | Scalar bits | EdDSA hash |
|-----|--------------|------------------|-------------|------------|
| `curve25519` | Ristretto (IsoEd25519) | 8 | 253 | SHA-512 |
| `ed448goldilocks` | Ed448-Goldilocks | 4 | 446 | SHAKE (via headers) |

Also encodes: Edwards parameter `d`, comb/WNAF window sizes, Montgomery base points, Ristretto base encodings, EdDSA domain separation strings.

### `template.py`

- Reads `*.tmpl.c`, `*.tmpl.h`, `*.tmpl.hxx`
- Substitutes `$(python_expression)` using curve/field data
- Emits Doxygen headers and include guards
- Invoked by CMake `add_custom_command` and Makefile rules

### Precomputed tables

`decaf_gen_tables` (built from `decaf_gen_tables.tmpl.c`) runs at build time to produce `decaf_tables.c`. Copies are **checked into** `src/curve25519/` and `src/ed448goldilocks/` to simplify cross-compilation; they refresh when dependencies change.

[← Project structure](03-project-structure.md) · [Next: Public API →](05-api.md)

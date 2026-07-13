# Quick start (C)

```c
#include <decaf.h>
#include <decaf/ed255.h>

uint8_t privkey[32], pubkey[32];
decaf_x25519_generate_key(privkey);
decaf_x25519_derive_public_key(pubkey, privkey);
```

Link with `-ldecaf` and set the include path to installed or generated `include/decaf/`.

For EdDSA signing, include `decaf/ed255.h` or `decaf/ed448.h` and use `decaf_ed25519_sign` / `decaf_ed448_sign` (see generated headers for buffer sizes).

Example build after install:

```bash
gcc -o x25519_demo demo.c -ldecaf -I/usr/local/include
```

## Building and validating EdDSA E-521

For the E-521 curve, the quickest path is the one-command runner at the repo
root. From `libdecaf/` (needs `cmake`, `make` and a C compiler):

```bash
./run_e521_tests.sh
```

It builds the library with the E-521 target, generates the base-point tables,
then compiles and runs both validation suites (`test/test_ed521_vectors.c` and
`test/test_ed521.c`). The public EdDSA API is `decaf/ed521.h` (66-byte keys,
132-byte signatures, SHAKE256). To build the library by hand, see the
["Evaluating the E-521 implementation" section of the root README](../../README.md)
or [Build & code generation](04-build-and-codegen.md).

[← Security & testing](07-security-and-testing.md) · [Index](README.md)

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

[← Security & testing](07-security-and-testing.md) · [Index](README.md)

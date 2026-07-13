# Decaf elliptic curve library

> **TCC fork — EdDSA over the E-521 curve.** This fork adds an experimental
> EdDSA implementation over the **E-521** twisted Edwards curve
> (field `p = 2^521-1`, cofactor 4, `d = -376014`), with 66-byte keys,
> 132-byte signatures and SHAKE256. See
> [`docs/en/README.md`](docs/en/README.md) (English) /
> [`docs/pt/README.md`](docs/pt/README.md) (Português) and
> [`ARCHITECTURE.md`](ARCHITECTURE.md).
>
> ## Evaluating the E-521 implementation (one command)
>
> From this directory, with `cmake`, `make` and a C compiler installed:
>
> ```bash
> ./run_e521_tests.sh
> ```
>
> This builds the library with the E-521 target, generates the base-point
> tables, then compiles and runs both validation suites:
>
> - **`test/test_ed521_vectors.c`** — reproduces byte-for-byte the 8 reference Ed521
>   test vectors from a previous UnB undergraduate thesis (TCC) — the reference
>   implementation this work builds on (public key, signature, verification;
>   messages of 0–1023 bytes).
> - **`test/test_ed521.c`** — checks `[k]·G` against an independent reference
>   (`test/e521_ref.py`, canonical neuromancer.sk parameters), plus 64 random
>   sign/verify round-trips and tamper rejection.
>
> To build the library manually instead of using the script:
>
> ```bash
> mkdir build && cd build
> cmake -DENABLE_EXPERIMENTAL_E521=ON -DCMAKE_BUILD_TYPE=Release ..
> make decaf-static        # build and link the library
> make decaf_tables_e521   # generate the base-point tables
> make decaf-static        # relink with the real tables
> ```
>
> The public API is `decaf/ed521.h`. The rest of this README is the upstream
> libdecaf documentation.
>
> ### Building the base library (without E-521)
>
> The stock library (Curve25519 / Ed448) builds with the upstream root Makefile:
>
> ```bash
> make lib          # produces build/lib/libdecaf.so
> ```
>
> Note: a bare `make` (the `all` target) also compiles the upstream *legacy test
> suite* (`test/test_decaf.cxx`, benches, …), written around 2017. On modern
> compilers some of those upstream test/header files trip newer `-Werror`
> diagnostics — this is unrelated to E-521 and affects stock libdecaf as well.
> For the base library use `make lib`; for E-521 use `./run_e521_tests.sh`.

---

The libdecaf library is for elliptic curve research and practical application.
It currently supports Ed448-Goldilocks and Curve25519.

The goals of this library are:

* Implementing the X25519, X448 key exchange protocols (RFC 7748).
* Implementing the Ed25519 and EdDSA-Ed448 signature schemes (RFC 8032).
* Providing a platform for research and development of advanced cryptographic schemes using twisted Edwards curves.

This library is intended for developers who have experience with
cryptography.  It doesn't (yet?) include documentation on how to use
digital signatures or key exchange securely.  Consult your local
cryptographer for advice.

## Mailing lists

Because this is new software, please expect it to have bugs, perhaps
even critical security bugs.  If you are using it, please sign up for
updates:

* Security-critical announcements (very low volume, God willing):
    decaf-security@googlegroups.com, join at https://groups.google.com/forum/#!forum/decaf-security    
* New version announcements (low volume):
    decaf-announce@googlegroups.com, join at https://groups.google.com/forum/#!forum/decaf-annonuce
* Library discussion (potentially more volume):
    decaf-discuss@googlegroups.com, join at https://groups.google.com/forum/#!forum/decaf-discuss

## General elliptic curve operations.

This is a multi-purpose elliptic curve library.  There is a C library,
and a set of C++ wrapper headers.  The C++ code consists entirely of
inline calls, and has no compiled component.

The library implements a fairly complete suite of operations on the
supported curves:

* Point and scalar serialization and deserialization.
* Point addition, subtraction, doubling, and equality.
* Point multiplication by scalars.  Accelerated double- and dual-scalar multiply.
* Scalar addition, subtraction, multiplication, division, and equality.
* Construction of precomputed tables from points.  Precomputed scalarmul.
* Hashing to the curve with an Elligator variant.  Inverse of elligator for steganography.  These are useful for advanced protocols such as password-authenticated key exchange (PAKE) and verifiable random functions (VRFs).

Internally, the library uses twisted Edwards curves with the "decaf"
and "ristretto" technique to remove the curve's cofactor of 4 or 8. 
The upshot is that systems using the "decaf" interface will be using
a prime-order group, which mitigates one of the few disadvantages of
Edwards curves.  However, this means that it is not able to implement
systems which care about cofactor information.

The goal of this library is not only to follow best practices, but to
make it easier for clients of the library to follow best practices.
With a few well-marked exceptions, the functions in this library should
be strongly constant-time: they do not allow secret data to flow to
array indices, nor to control decisions except for a final failure
check.  Furthermore, the C++ wrapping uses RAII to automatically clear
sensitive data, and has interfaces designed to prevent certain mistakes.

## CFRG cryptosystems.

The library additionally supports two cryptosystems defined by the
Crypto Forum Research Group (CFRG): the X448/X25519 Diffie-Hellman
functions (RFC 7748), and the EdDSA signature scheme (RFC 8032).
Future versions might support additional operations on these curves,
such as precomputed signature verification.

## Symmetric crypto and hashing

The Decaf library doesn't implement much symmetric crypto, but it does
contain the hash functions required by the CFRG cryptosystems: SHA512,
SHA-3 and SHAKE.

## Internals

The "decaf" technique is described in https://eprint.iacr.org/2015/673
While the title of that paper is "removing cofactors through point
compression", it might be more accurate to say "through quotients and
isogenies".  The internal representation of points is as "even" elements
of a twisted Edwards curve with a=-1.  Using this subgroup removes a
factor of 2 from the cofactor.  The remaining factor of 2 or 4 is
removed with a quotient group: any two points which differ by an element
of the 2- or 4-torsion subgroup are considered equal to each other.

When a point is written out to wire format, it is converted (by isogeny)
to a Jacobi quartic curve, which is halfway between an Edwards curve
and a Montgomery curve.  One of the 4 or 8 equivalent points on the
Jacobi quartic is chosen (it is "distinguished" according to certain
criteria, such as having a positive x-coordinate).  The x-coordinate of
this point is written out.  The y-coordinate is not written out, but the
decoder knows which of the two possible y-coordinates is correct because
of the distinguishing rules.  See the paper for more details.

As of v0.9.4, libdecaf uses the "Ristretto" variant of this encoding.
See https://www.ristretto.group for details, once that site is up.

## Build and Install

 cmake -DCMAKE_INSTALL_PREFIX=<Install path> <path to root directory>
 make
 make test
 make install

Most C source code is generated through a python script during the build.
Some files holding tables are generated in one more step building an
executable to generate them. They are thus stored in the source tree to help
cross-compilation. The build script update them when their dependencies
are modified, to build only these files:

 make decaf_tables

Doxygen generated documentation is located in ./doc directory in the
binary tree after running

 make doc

## Licensing

Most of the source files here are by Mike Hamburg.  Those files are (c)
2014-2017 Cryptography Research, Inc (a division of Rambus). All of these
files are usable under the MIT license contained in LICENSE.txt.

## Caveats

As mentioned in the license, there is absolutely NO WARRANTY on any of this
code.  This code might well have security-critical bugs despite my best efforts.

I've attempted to protect against timing attacks and invalid point attacks,
but as of yet I've made no attempt to protect against power analysis.


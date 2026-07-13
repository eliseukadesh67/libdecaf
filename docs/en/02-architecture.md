# Architecture

## High-level layers

```mermaid
flowchart TB
    subgraph apps [Applications]
        APP[C/C++ programs]
        PY[Python edgold]
    end

    subgraph api [Public API]
        CXX["C++ headers (*.hxx)<br/>inline wrappers, RAII"]
        CAPI["C headers (decaf.h, point_*.h, ed*.h)"]
    end

    subgraph protocols [CFRG protocols]
        X255["X25519 / X448"]
        EDDSA["Ed25519 / Ed448 EdDSA"]
    end

    subgraph curve [Per-curve logic - generated]
        DECAF["decaf.c - group ops, encode/decode"]
        ELL["elligator.c"]
        SCAL["scalar.c"]
        EDD["eddsa.c"]
        TBL["decaf_tables.c - precomputed base"]
    end

    subgraph field [Prime fields]
        P255["p25519: 2^255-19"]
        P448["p448: Goldilocks 2^448-2^224-1"]
    end

    subgraph arch [Architecture-specific]
        FIMPL["f_impl.c - SIMD / intrinsics"]
        FARITH["f_arithmetic.c"]
        FGEN["f_generic.c - generated"]
    end

    subgraph common [Shared utilities]
        CT["constant_time.h"]
        HASH["sha512, shake, spongerng"]
        UTIL["utils.c"]
    end

    APP --> CXX
    APP --> CAPI
    PY --> CAPI
    CXX --> CAPI
    CAPI --> protocols
    CAPI --> curve
    protocols --> curve
    curve --> field
    field --> arch
    curve --> common
    field --> common
```

## Data flow: point encoding (Ristretto)

Internally, points live on a twisted Edwards curve with `a = -1`, in extended homogeneous coordinates `(x, y, z, t)`. Wire format uses the **Ristretto** encoding (v0.9.4+):

1. Map to an isogenous **Jacobi quartic** curve.
2. Pick a **distinguished** representative among 4 or 8 equivalent points (sign rules on coordinates).
3. Serialize the **x-coordinate** only (32 bytes for 255-bit curves, 57 for Ed448).

This yields a canonical, cofactor-free byte string per group element.

## Build-time code generation pipeline

```mermaid
flowchart LR
    CD["curve_data.py<br/>curve + field parameters"]
    TP["template.py<br/>$(...) substitution"]
    TMPL["*.tmpl.c / *.tmpl.h / *.tmpl.hxx"]
    GEN["src/GENERATED/<br/>headers + C sources"]
    GT["decaf_gen_tables<br/>executable"]
    TBL["decaf_tables.c<br/>committed in tree"]

    CD --> TP
    TMPL --> TP
    TP --> GEN
    GEN --> GT
    GT --> TBL
    GEN --> LIB["libdecaf.so / libdecaf.a"]
    TBL --> LIB
```

Most curve- and field-specific C/H files are **not** hand-maintained per curve; they are instantiated from templates using parameters in `src/generator/curve_data.py`.

[← Overview](01-overview.md) · [Next: Project structure →](03-project-structure.md)

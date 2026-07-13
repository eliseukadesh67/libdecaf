# Arquitetura

## Camadas de alto nível

```mermaid
flowchart TB
    subgraph apps [Aplicações]
        APP[Programas C/C++]
        PY[Python edgold]
    end

    subgraph api [API pública]
        CXX["Cabeçalhos C++ (*.hxx)<br/>wrappers inline, RAII"]
        CAPI["Cabeçalhos C (decaf.h, point_*.h, ed*.h)"]
    end

    subgraph protocols [Protocolos CFRG]
        X255["X25519 / X448"]
        EDDSA["Ed25519 / Ed448 EdDSA"]
    end

    subgraph curve [Lógica por curva — gerada]
        DECAF["decaf.c — operações de grupo"]
        ELL["elligator.c"]
        SCAL["scalar.c"]
        EDD["eddsa.c"]
        TBL["decaf_tables.c — base pré-computada"]
    end

    subgraph field [Corpos primos]
        P255["p25519: 2^255-19"]
        P448["p448: Goldilocks 2^448-2^224-1"]
    end

    subgraph arch [Específico da arquitetura]
        FIMPL["f_impl.c — SIMD / intrínsecos"]
        FARITH["f_arithmetic.c"]
        FGEN["f_generic.c — gerado"]
    end

    subgraph common [Utilitários compartilhados]
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

## Fluxo de codificação de pontos (Ristretto)

Internamente, os pontos usam coordenadas homogêneas estendidas `(x, y, z, t)` em uma curva de Edwards torcida com `a = -1`. O formato em rede (*wire format*) segue a codificação **Ristretto** (a partir da v0.9.4):

1. Mapeamento por isogenia para uma curva **quártica de Jacobi**.
2. Escolha de um representante **distinguido** entre 4 ou 8 pontos equivalentes (regras de sinal nas coordenadas).
3. Serialização apenas da **coordenada x** (32 bytes na curva de 255 bits; 57 bytes no Ed448).

O resultado é uma sequência de bytes canônica por elemento do grupo, sem ambiguidade de cofator.

## Pipeline de geração de código na compilação

```mermaid
flowchart LR
    CD["curve_data.py<br/>parâmetros de curva e campo"]
    TP["template.py<br/>substituição $(...)"]
    TMPL["*.tmpl.c / *.tmpl.h / *.tmpl.hxx"]
    GEN["src/GENERATED/<br/>cabeçalhos e fontes C"]
    GT["decaf_gen_tables<br/>executável"]
    TBL["decaf_tables.c<br/>versionado no repositório"]

    CD --> TP
    TMPL --> TP
    TP --> GEN
    GEN --> GT
    GT --> TBL
    GEN --> LIB["libdecaf.so / libdecaf.a"]
    TBL --> LIB
```

A maior parte do código específico por curva **não** é mantida manualmente para cada curva: instancia-se a partir de modelos (*templates*) parametrizados por `src/generator/curve_data.py`.

[← Visão geral](01-visao-geral.md) · [Próximo: Estrutura do projeto →](03-estrutura-do-projeto.md)

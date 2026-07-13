# Compilação e geração de código

Há duas formas equivalentes de compilar: **CMake** (recomendado para instalação e empacotamento) e **Makefile** (usado pelo wrapper Python e por parte dos desenvolvedores).

## CMake

```bash
cmake -DCMAKE_INSTALL_PREFIX=/caminho/de/instalacao /caminho/para/libdecaf
make
make test          # CTest: decaf_tester, bench, etc.
make install
make doc           # Doxygen (requer generatedCode)
```

### Opções do CMake

| Opção | Padrão | Descrição |
|--------|---------|-----------|
| `ENABLE_SHARED` | ON | Gera `libdecaf.so` |
| `ENABLE_STATIC` | ON | Gera `libdecaf.a` |
| `ENABLE_STRICT` | YES | `-Werror` e avisos extras |
| `ENABLE_TESTS` | YES | Compila e registra testes |
| `GENERATED_SOURCE_PATH` | OFF | Caminho customizado para fontes geradas |

### Seleção automática de arquitetura

| `CMAKE_SYSTEM_PROCESSOR` | Intrínsecos globais | p25519 | p448 |
|--------------------------|---------------------|--------|------|
| `x86_64` | `arch_x86_64` | `arch_x86_64` | `arch_x86_64` |
| `aarch64` / `arch64` | `arch_ref64` | `arch_ref64` | `arch_ref64` |
| `arm` (+ NEON no Android) | `arch_neon` ou `arch_arm_32` | `arch_32` | `arch_neon` / `arch_arm_32` |
| padrão | `arch_32` | `arch_32` | `arch_32` |

Para regenerar apenas as tabelas pré-computadas:

```bash
make decaf_tables
```

## GNU Make

```bash
make              # libdecaf.so + binários de teste internos
make test
make bench
make doc
make clean
```

As saídas ficam em `build/lib`, `build/bin`, com código gerado espelhado em `src/GENERATED/`.

> Nota: o scaffold experimental de E-521 foi integrado no pipeline **CMake** neste primeiro passo. O caminho via `Makefile` ainda não inclui `p521/e521`.

### Erros comuns com compiladores recentes (GCC/Clang)

Em toolchains novas, o build pode falhar porque o `Makefile` usa `-Werror` e transforma avisos em erro. Dois casos comuns no `libdecaf` são:

- `-Woverflow` em `decaf/common.h`
- `-Wcatch-value` em `test/test_decaf.cxx`

Workaround recomendado para compilar e testar sem alterar código-fonte:

```bash
make clean
make EXWARN="-Wno-error=overflow -Wno-error=catch-value"
make test EXWARN="-Wno-error=overflow -Wno-error=catch-value"
```

Se ainda houver falha por variação de versão do compilador:

```bash
make clean
make EXWARN="-Wno-error=overflow -Wno-overflow -Wno-error=catch-value -Wno-catch-value"
make test EXWARN="-Wno-error=overflow -Wno-overflow -Wno-error=catch-value -Wno-catch-value"
```

Para apenas gerar a biblioteca (sem compilar testes):

```bash
make clean
make lib EXWARN="-Wno-error=overflow"
```

## Artefato da biblioteca

- **Alvos:** `decaf` (compartilhada), `decaf-static` (estática)
- **Bibliotecas objeto:** `p25519`, `p448`, `CURVE25519`, `CURVE448` ligadas em um único `libdecaf`
- **Instalação:** cabeçalhos em `include/decaf/`, config CMake em `share/decaf/cmake`

## Geração de código

### `curve_data.py`

Arquivo central de parâmetros para a geração.

**Campos:**

| Chave | Módulo | Usado por |
|-------|--------|-----------|
| `p25519` | 2^255 − 19 | curve25519 |
| `p448` | 2^448 − 2^224 − 1 | ed448goldilocks |

**Curvas:**

| Chave | Nome | Cofator removido | Bits do escalar | Hash EdDSA |
|-------|------|------------------|-----------------|------------|
| `curve25519` | Ristretto (IsoEd25519) | 8 | 253 | SHA-512 |
| `ed448goldilocks` | Ed448-Goldilocks | 4 | 446 | SHAKE |

Também define: parâmetro `d` de Edwards, tamanhos de janela comb/WNAF, pontos base Montgomery/Ristretto, strings de separação de domínio EdDSA.

### `template.py`

- Lê `*.tmpl.c`, `*.tmpl.h`, `*.tmpl.hxx`
- Substitui expressões `$(...)` com dados da curva/campo
- Gera cabeçalhos Doxygen e *include guards*
- Invocado por regras CMake e Makefile

### Tabelas pré-computadas

O executável `decaf_gen_tables` (a partir de `decaf_gen_tables.tmpl.c`) produz `decaf_tables.c`. Cópias ficam em `src/curve25519/` e `src/ed448goldilocks/` para facilitar compilação cruzada; são atualizadas quando as dependências mudam.

[← Estrutura do projeto](03-estrutura-do-projeto.md) · [Próximo: API pública →](05-api.md)

# Implementação do EdDSA E‑521 na libdecaf — Guia passo a passo

> Documento de engenharia do TCC 2 (Eliseu Kadesh, UnB/FCTE).
> Objetivo: descrever, de forma didática e detalhada, **o que precisa ser feito**
> para ter o EdDSA sobre a curva **E‑521** funcionando na `libdecaf`, **o que já
> foi feito**, e **onde estamos travados** — para que o processo fique
> compreensível e auditável.

---

## Sumário

1. [Como a libdecaf é organizada (visão geral)](#1-como-a-libdecaf-é-organizada)
2. [O que o EdDSA E‑521 precisa: as 3 camadas](#2-o-que-o-eddsa-e-521-precisa-as-3-camadas)
3. [Estado atual — o que já está pronto](#3-estado-atual--o-que-já-está-pronto)
4. [Os passos restantes (detalhados)](#4-os-passos-restantes-detalhados)
5. [O ponto onde estamos travados (ponto base)](#5-o-ponto-onde-estamos-travados-ponto-base)
6. [Plano de validação (oráculo ECPy)](#6-plano-de-validação-oráculo-ecpy)
7. [Referência rápida: arquivos e comandos](#7-referência-rápida-arquivos-e-comandos)
8. [Glossário](#8-glossário)

---

## 1. Como a libdecaf é organizada

A `libdecaf` **não é escrita curva por curva à mão**. Ela tem um **gerador de
código em Python** que produz o código C de cada curva a partir de um único
arquivo de parâmetros. Entender essa separação é a chave para tudo.

Existem **duas grandes partes**:

### (A) Backend de corpo (aritmética do campo) — escrito à mão
Fica em `src/pXXX/` (ex.: `src/p448/`, `src/p521/`). É onde vivem as operações
com números módulo o primo `p` do corpo: multiplicação, quadrado, adição,
subtração, redução, raiz inversa. É **específico do primo** e otimizado à mão.

- `src/p521/` → aritmética de `p = 2^521 − 1` (o nosso corpo).
- Cada backend tem variações por arquitetura: `arch_ref64/` (portátil 64 bits),
  `arch_x86_64/` (otimizado Intel/AMD), `arch_32/` (32 bits).
- **Na sua máquina (x86_64) o build usa `arch_x86_64/`.**

### (B) Camada de curva + protocolo — gerada automaticamente
Fica em `src/per_curve/*.tmpl.c` (modelos) e é transformada em código C pelo
gerador. Produz, para cada curva:

- `decaf.c` — aritmética de **pontos** (soma, dobra, multiplicação escalar,
  codificação/decodificação de pontos).
- `scalar.c` — aritmética **dos escalares** (mod a ordem do grupo `L`).
- `eddsa.c` — o **protocolo EdDSA** (gerar chave, assinar, verificar).
- `elligator.c` — mapa hash‑para‑curva do **Ristretto** (NÃO é usado pelo EdDSA).
- `decaf_tables.c` — **tabelas pré‑computadas do ponto base** (para acelerar a
  multiplicação escalar pela base).

### O “cérebro”: `src/generator/curve_data.py`
Esse arquivo é um dicionário Python com **os parâmetros de cada corpo e de cada
curva**. O gerador lê daqui os números (primo, `d`, ordem, ponto base…) e
substitui nos modelos. Ou seja: **mudar um parâmetro da curva = editar este
arquivo e regerar**.

```
curve_data.py  ──(gerador Python)──►  decaf.c, scalar.c, eddsa.c, elligator.c
     ▲                                          │
     │ parâmetros (p, d, ordem, base…)          │ usa o backend de corpo…
     └──────────────────────────────────────────┴──►  src/p521/  (à mão)
```

### O fluxo de build (CMake)
```
cmake -DENABLE_EXPERIMENTAL_E521=ON ..     # liga o alvo experimental E-521
make decaf-static                          # gera o código, compila tudo, linka a lib
make decaf_tables_e521                     # (passo especial) gera as tabelas do ponto base
```

---

## 2. O que o EdDSA E‑521 precisa: as 3 camadas

Para o EdDSA E‑521 funcionar de ponta a ponta, precisamos das três camadas
**corretas e coerentes entre si**:

| Camada | O que é | Onde fica | Status |
|---|---|---|---|
| 1. Aritmética de corpo | operações mod `2^521−1` | `src/p521/` (à mão) | ✅ feito |
| 2. Parâmetros da curva | `d`, ordem `L`, ponto base | `src/generator/curve_data.py` | ⚠️ parcial |
| 3. Tabelas do ponto base | pré‑cálculo da base | `src/e521/decaf_tables.c` (gerado) | ❌ falta |

Se **qualquer** uma estiver errada, as assinaturas **não** vão bater com o ECPy.
Por isso a ordem importa: primeiro o corpo, depois os parâmetros, por último as
tabelas — e então a validação.

### Parâmetros canônicos do E‑521 (fonte: neuromancer.sk + ECPy)
- Primo do corpo: `p = 2^521 − 1`
- Curva de Edwards: `a = 1`, `d = −376014` (≡ `p − 376014 mod p`)
- Cofator: `h = 4`
- Ordem do subgrupo primo: `L = 2^519 − 337554763258501705789107630418782636071904961214051226618635150085779108655765`
- Ponto base: `Gx = 0x752cb45c…a2f19ba6c`, `Gy = 12`
- Hash do EdDSA: **SHAKE256**, domínio `"SigEd521"`
- Tamanhos: chave privada/pública = **66 bytes**, assinatura = **132 bytes**

---

## 3. Estado atual — o que já está pronto

### ✅ Passo 1 — Aritmética de corpo `p521` (FEITO e verificado)
- **Origem:** existia uma implementação histórica pronta em
  `_aux/attic/p521/arch_ref64/` (do autor original da decaf, Mike Hamburg).
  Ela estava fora do build (“attic” = sótão).
- **O que foi feito:** portamos `gf_mul`, `gf_sqr`, `gf_mulw`, `gf_add/sub`,
  `gf_weak_reduce` e `gf_isr` para `src/p521/arch_ref64/` **e** `arch_x86_64/`,
  além de `src/p521/f_arithmetic.c`.
- **Adaptações necessárias** (a interface da lib mudou desde o “attic”):
  - Adicionados os macros `GF_HEADROOM` e `FIELD_LITERAL` (9 “limbs”/dígitos).
  - `gf_mulw` → `gf_mulw_unsigned` (interface nova).
  - `gf_isr` reescrita para a assinatura atual `mask_t gf_isr(...)` (retorna se
    a raiz existe). **Verificamos matematicamente** que a cadeia de operações
    calcula exatamente `x^((p−3)/4)`, que é a raiz quadrada inversa correta para
    `p ≡ 3 (mod 4)`.
  - **Bug sutil de arquitetura corrigido:** no x86_64 desta lib, a macro
    `widemul` recebe **ponteiros**; a versão valor‑valor chama‑se `widemul_rr`.
    Criamos uma macro `WIDEMUL` que escolhe a certa por arquitetura.
- **Resultado:** compila **sem warnings**.

### ✅ Passo 2a — Ordem do grupo (trace) (FEITO e verificado)
- No `curve_data.py`, o campo `trace` estava com **valor de placeholder (`1`)**.
- O gerador calcula a ordem do subgrupo como `q = (p + 1 − trace) / h`. Com
  `trace = 1`, `q` ficava **errado** → escalares errados → assinatura nunca
  fecharia.
- Calculamos o `trace` real a partir de `L`:
  `trace = p + 1 − h·L = 0xba924e6e2e40de823251d428604eb03ec109ceea595c382eafc576f282b9fca54`.
- Verificado: agora `q == L` (ordem de 519 bits, batendo com `scalar_bits`).

### ✅ A biblioteca **compila e linka** com o E‑521 ligado
- `cmake -DENABLE_EXPERIMENTAL_E521=ON` + `make decaf-static` → **sucesso**.
- `decaf.c`, `scalar.c`, `eddsa.c` do E‑521 são gerados e compilados.
- `DECAF_EDDSA_521_PUBLIC_BYTES = 66` (correto).

### ⚠️ Decisão de escopo: `elligator.c` foi EXCLUÍDO do build E‑521
- O `elligator.c` (mapa hash‑para‑curva do Ristretto) tem um **`#error`
  proposital** no próprio código‑fonte original para o corpo `p521`
  (a condição `521 == 8·SER_BYTES + 1` exige um ajuste de “bit alto” que **nunca
  foi terminado no upstream**).
- **O EdDSA não usa elligator.** Ele é usado só pelo Ristretto (hash‑para‑ponto).
- Por isso, `src/e521/CMakeLists.txt` foi editado para **não compilar**
  `elligator.c`. Está documentado no arquivo. Isso está dentro do escopo do TCC
  (que é EdDSA, não Ristretto).

### ❌ Passo 2b/3 — Tabelas do ponto base (FALTA — é onde estamos)
- `src/e521/decaf_tables.c` ainda é um **stub** (3 linhas de comentário).
- Sem as tabelas corretas do ponto base, `gerar chave / assinar / verificar`
  não produzem resultados corretos. **Este é o trabalho restante** e está
  detalhado nas seções 4 e 5.

---

## 4. Os passos restantes (detalhados)

### Passo 3.1 — Definir o `rist_base` (codificação do ponto base)

**Contexto.** O gerador de tabelas (`decaf_gen_tables.c`) constrói as tabelas a
partir de **um ponto base já codificado**, guardado no `curve_data.py` no campo
`rist_base`. Para as outras curvas (ex.: ed448) esse valor é um número conhecido
e publicado. **Para o E‑521 não existe valor publicado** — precisamos derivá‑lo.

**Por que não é só “usar (Gx, 12)”.** A libdecaf, internamente, não trabalha
diretamente com o ponto `(Gx, 12)` do EdDSA. Ela usa uma **isogenia de grau 4**
para levar o ponto para uma curva “torcida” interna (é o truque do Decaf que
elimina o cofator). Por causa disso, o ponto guardado nas tabelas
(`point_base`) **não é** `(Gx, 12)`, e sim um ponto relacionado por um fator.

**A relação exata (deduzida do código):**
- `DECAF_521_EDDSA_ENCODE_RATIO = 4` e `DECAF_521_EDDSA_DECODE_RATIO = 1`.
- Codificar depois de decodificar multiplica por 4 (o cofator).
- Logo, o ponto das tabelas deve ser:
  ```
  point_base = inv(4) · decode_like_eddsa(B)
  ```
  onde `B` é o gerador `(Gx, 12)` na codificação EdDSA e `inv(4)` é o inverso de
  4 módulo `L`.

**Como derivar sem Sage (Sage não está instalado):** usamos a **própria lib já
compilada** para fazer a conta. Existe um helper: `gen_e521_base.c`, que:
1. monta a codificação EdDSA de `(Gx, 12)` (66 bytes: `y=12` em little‑endian +
   bit de sinal de `x`);
2. chama `decode_like_eddsa_and_mul_by_ratio(B)`;
3. multiplica por `inv(4) mod L`;
4. chama `point_encode(...)` → obtém os 65 bytes do `rist_base`.

**Onde colocar o resultado:** no `src/generator/curve_data.py`, campo
`"rist_base"` da curva `e521` (string hexadecimal de 65 bytes = 130 caracteres).

> ⚠️ **É aqui que estamos travados.** Ver seção 5 para o detalhe do problema
> atual (round‑trip encode/decode) e as opções de resolução.

### Passo 3.2 — Gerar as tabelas do ponto base

Com o `rist_base` correto no `curve_data.py`:

```bash
cd libdecaf/build-e521-new
cmake ..                        # regenera o código a partir do curve_data.py
make decaf_tables_e521          # roda o gerador de tabelas
```

Isso executa `decaf_gen_tables_e521`, que **decodifica** o `rist_base`, calcula
os múltiplos pré‑computados da base e **escreve** o resultado em
`src/e521/decaf_tables.c` (deixará de ser um stub e passará a ter centenas de
linhas de constantes).

**Como verificar:** `wc -l src/e521/decaf_tables.c` deve mostrar um arquivo
grande (não 3 linhas), começando com `const decaf_521_point_t decaf_521_point_base = …`.

### Passo 3.3 — Recompilar a biblioteca com as tabelas reais

```bash
make decaf-static      # (e/ou o alvo da .so, se quiser a biblioteca dinâmica)
```

Agora a lib tem `decaf_521_point_base` e as tabelas `precomputed_*` corretas.

### Passo 3.4 — Escrever um teste C de EdDSA

Um pequeno programa (ex.: `test_ed521.c`) que usa a API pública gerada
(`decaf/ed521.h`):

- `decaf_ed521_derive_public_key(pub, priv)` — gera a chave pública.
- `decaf_ed521_sign(sig, priv, pub, msg, len, 0, NULL, 0)` — assina.
- `decaf_ed521_verify(sig, pub, msg, len, 0, NULL, 0)` — verifica.

### Passo 3.5 — Validar contra os vetores do ECPy (ver seção 6)

Comparar **byte a byte**:
- `pub` gerada pela libdecaf × `KNOWN_PUB` do ECPy;
- `sig` gerada pela libdecaf × `KNOWN_SIG` do ECPy;
- e o **interop**: assinar na libdecaf e **verificar no ECPy**, e vice‑versa.

Se bater, a implementação está **matematicamente correta** e o objetivo do TCC
(geração de chave, assinatura e verificação do EdDSA E‑521) está cumprido.

---

## 5. O ponto onde estamos travados (ponto base)

### O sintoma
Ao rodar o gerador de tabelas, ele falha com **`Can't decode base point!`** —
ou seja, o `rist_base` que derivamos **não é aceito** pela função
`point_decode` na hora de gerar as tabelas.

### O que já sabemos (diagnóstico até agora)
- O helper `gen_e521_base.c` **produz** um `rist_base` candidato
  (`ba2cf50c…ea484e`).
- No **próprio helper**, se pegamos o ponto, **codificamos e decodificamos de
  volta**, o round‑trip **funciona** (decodifica e o ponto volta igual).
- **Mas**, ao decodificar os **mesmos bytes escritos como literais**, a
  decodificação **falha**.
- Conclusão parcial: há uma **inconsistência entre o que `point_encode`
  imprime e o que `point_decode` aceita** para o corpo `p521`. Isso é
  **fortemente relacionado à mesma limitação não terminada do `elligator`**: o
  corpo de 521 bits tem **1 bit “sobrando”** (`SER_BYTES = 65` bytes = 520 bits,
  mas o elemento tem até 521 bits), e o tratamento desse bit no
  encode/decode do Decaf para p521 **não foi finalizado no upstream**.

### Por que isso acontece (a raiz do problema)
A codificação **Decaf** (`point_encode`/`point_decode`) foi feita e testada para
corpos onde o elemento cabe “folgado” nos bytes (ed25519, ed448). No E‑521 o
elemento **quase preenche** os bytes, sobrando exatamente 1 bit. O código do
upstream **marca esse caso com `#error`** (que já contornamos no elligator) e,
aparentemente, o caminho de encode/decode do Decaf **também** é afetado por essa
mesma pendência.

> **Importante:** essa pendência é do **Decaf/Ristretto** (a codificação interna
> de ponto de 65 bytes), **não** do EdDSA. O EdDSA usa a **sua própria**
> codificação de **66 bytes** (`encode_like_eddsa`/`decode_like_eddsa`), que
> **compilou e trata o bit alto corretamente** (é por isso que a chave pública
> tem 66 bytes, não 65).

### As opções de resolução (a decidir)

**Opção A — Consertar o encode/decode do Decaf para p521 (mais “correto”, mais
trabalho).**
Implementar o ajuste do bit alto que falta em `point_encode`/`point_decode`
(análogo ao que o `#error` do elligator pedia). Assim `rist_base` volta a
funcionar pelo caminho normal do gerador. Risco: mexer em código gerado/gerador;
exige cuidado para não quebrar as outras curvas.

**Opção B — Contornar o `rist_base` no gerador de tabelas (mais direto).**
Modificar o `decaf_gen_tables.c` (só para o E‑521) para **construir o ponto base
diretamente** via `decode_like_eddsa_and_mul_by_ratio((Gx,12)) · inv(4)` —
usando a codificação **EdDSA de 66 bytes** (que funciona), **sem passar** pela
codificação Decaf de 65 bytes (que está com a pendência). Isso evita o encode/
decode problemático por completo e é o caminho **mais rápido** para ter tabelas
corretas hoje.

**Opção C — Gerar as tabelas com um programa próprio.**
Escrever um pequeno gerador de tabelas independente que use as funções internas
(`precompute`, `precompute_wnafs`) a partir do ponto base construído como na
Opção B, e imprimir o `decaf_tables.c`. Equivalente à B, porém fora do template.

> **Recomendação:** começar pela **Opção B** — é a que menos toca em código
> sensível e chega mais rápido a tabelas válidas. Se der certo na validação
> (seção 6), o objetivo do TCC está cumprido. A Opção A pode ficar como
> “trabalho futuro” (finalizar o suporte Ristretto ao E‑521), o que inclusive
> rende texto para a monografia.

---

## 6. Plano de validação (oráculo ECPy)

A metodologia do TCC (seção 3.1.4) usa o **ECPy como oráculo de verdade**. E a
boa notícia: **os vetores já existem** no seu `ECPy/test.py`, na curva `Ed521`,
com hash SHAKE256:

| Vetor | Valor (início) | Tamanho |
|---|---|---|
| `KNOWN_PRIV` | `319e7537…d922` | 66 bytes |
| `KNOWN_PUB`  | `e95be773…6501` | 66 bytes |
| `KNOWN_MSG`  | `03` | 1 byte |
| `KNOWN_SIG`  | `31a8284a…3d00` | 132 bytes |

**Roteiro de validação:**
1. **Chave pública:** `derive_public_key(KNOWN_PRIV)` na libdecaf deve dar
   **exatamente** `KNOWN_PUB`. *(Este é o teste que decide se o ponto base está
   certo — se a pública bater, a base está correta.)*
2. **Assinatura:** `sign(KNOWN_PRIV, KNOWN_PUB, MSG=03)` deve dar `KNOWN_SIG`.
3. **Verificação:** `verify(KNOWN_SIG, KNOWN_PUB, MSG=03)` deve retornar sucesso.
4. **Interoperabilidade:** assinar uma mensagem nova na libdecaf e **verificar no
   ECPy**; depois assinar no ECPy e **verificar na libdecaf**.

Se o passo 1 falhar por um **fator constante** (ex.: pública é `4×` ou `¼` da
esperada), o ajuste é no `inv(4)` da derivação do ponto base (candidato
`rist_base_inv4` vs `rist_base_noinv` já calculados pelo helper).

---

## 7. Referência rápida: arquivos e comandos

### Arquivos que importam
```
src/p521/arch_x86_64/f_impl.{c,h}   # aritmética de corpo (USADA no build x86_64) — PRONTO
src/p521/arch_ref64/f_impl.{c,h}    # idem, portátil                              — PRONTO
src/p521/f_arithmetic.c             # MODULUS/gf_isr do corpo                     — PRONTO
src/generator/curve_data.py         # parâmetros da curva (trace PRONTO; rist_base FALTA)
src/e521/CMakeLists.txt             # build do E-521 (elligator excluído)         — PRONTO
src/e521/decaf_tables.c             # tabelas do ponto base                       — FALTA (stub)
gen_e521_base.c                     # helper que deriva o rist_base               — em uso
IMPLEMENTACAO_E521.md               # este documento
```

### Comandos
```bash
# 1) Configurar e compilar a biblioteca com E-521
cd libdecaf && mkdir -p build-e521-new && cd build-e521-new
cmake -DENABLE_EXPERIMENTAL_E521=ON -DCMAKE_BUILD_TYPE=Release ..
make decaf-static

# 2) (Passo travado) Derivar o rist_base com o helper
cd ..
gcc -O2 gen_e521_base.c -I build-e521-new/src/GENERATED/include \
    build-e521-new/src/libdecaf.a -o gen_e521_base
./gen_e521_base            # imprime rist_base_inv4 / rist_base_noinv

# 3) Colar o rist_base em src/generator/curve_data.py (curva e521), depois:
cd build-e521-new && cmake .. && make decaf_tables_e521

# 4) Recompilar e validar
make decaf-static

# 5) Rodar o ECPy (oráculo) para comparar
cd ../../ECPy && venv/bin/python test.py
```

---

## 8. Glossário

- **Corpo (field):** os números módulo `p`. Aqui `p = 2^521 − 1` (primo de
  Mersenne — permite redução rápida por deslocamento de bits).
- **limb:** um “dígito” do número grande. O E‑521 usa **9 limbs** de 58/57 bits.
- **`gf_*`:** funções da aritmética de corpo (`gf_mul`, `gf_sqr`, …).
- **`gf_isr`:** *inverse square root* — calcula `1/√x`, usada na decodificação de
  pontos e em normalizações.
- **Cofator (h):** razão entre o número total de pontos da curva e a ordem `L` do
  subgrupo primo. No E‑521, `h = 4`.
- **Ordem `L`:** tamanho do subgrupo primo onde o EdDSA opera; os escalares são
  reduzidos módulo `L`.
- **trace:** parâmetro `t = p + 1 − #E`. O gerador o usa para achar `L`.
- **Decaf/Ristretto:** técnica que “esconde” o cofator, representando pontos de
  forma canônica. Tem **sua própria** codificação (65 bytes no E‑521), diferente
  da codificação EdDSA (66 bytes).
- **`rist_base`:** o ponto base **codificado** que o gerador de tabelas lê.
- **isogenia de grau 4 / ratio:** o mapeamento entre a curva EdDSA “de fora” e a
  curva torcida interna do Decaf; explica por que `point_base ≠ (Gx,12)` e por que
  aparece o fator `4` (`ENCODE_RATIO`).
- **ECPy:** biblioteca Python de referência; nosso **oráculo** de corretude.

*Fim do documento.*

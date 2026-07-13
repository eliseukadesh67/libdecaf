# Projeto criptográfico

## Eliminação de cofator Decaf / Ristretto

As curvas de Edwards torcidas usadas têm cofator **4** (Ed448) ou **8** (Curve25519). A biblioteca:

1. Trabalha no **subgrupo par** da curva de Edwards (remove um fator 2).
2. Faz quociente pela **2- ou 4-torsão** restante, de modo que a codificação em rede representa classes de equivalência, não pontos brutos da curva.

As aplicações enxergam um **grupo de ordem prima** (tamanho ≈ p), o que simplifica protocolos e evita armadilhas clássicas de cofator.

## Nomenclatura Curve25519 e Ristretto

- Curva interna: **IsoEd25519** (isógena a Ed25519 / Curve25519).
- Codificação em rede: **Ristretto** (alinhada a [ristretto.group](https://ristretto.group); mudança em relação à codificação Decaf antiga na v0.9.4 — ver [HISTORY.txt](../../HISTORY.txt)).
- Classe C++ renomeada para **Ristretto**; `IsoEd25519` permanece como sinônimo.

## Coordenadas e fórmulas

- Coordenadas **estendidas de Edwards torcida** `(x, y, z, t)` com `t = xy/z`.
- Fórmulas visam ser **completas** (adição sem casos especiais).
- A decodificação ainda pode **falhar** (sequências inválidas); caminhos de assinatura/verificação devem checar valores de retorno.

## Elligator

`elligator.c` (por curva) implementa mapeamento hash-para-curva e inversos, úteis em PAKE, VRF e codificações esteganográficas.

## Estratégia de multiplicação escalar

Configurada em `curve_data.py` por curva:

- Método **comb** (parâmetros n, t, s)
- Tamanhos de tabela **wNAF** fixo/variável
- **Bits de janela** para buscas em tabela em tempo constante
- Tabela da **base** pré-computada em `decaf_tables.c`

## Plataforma e desempenho

Cada corpo primo possui:

- `f_arithmetic.c` — operações modulares centrais
- `f_impl.c` — otimizado por CPU (`arch_x86_64`, `arch_neon`, etc.)
- `f_generic.c` — fallback portátil gerado

A maior parte do código específico de plataforma está em **`f_impl.c`** (centenas de linhas por arquitetura, sobretudo NEON).

### Limitações conhecidas (notas upstream)

- Caminho ARM NEON historicamente ~15–20% mais lento que o Goldilocks isolado anterior; x86_64 praticamente inalterado
- Escada de Montgomery removida (pode afetar alguns caminhos estilo X25519)
- Verificação formal e testes exaustivos de cantos ainda em aberto ([TODO.txt](../../TODO.txt))

[← API pública](05-api.md) · [Próximo: Segurança e testes →](07-seguranca-e-testes.md)

# libdecaf — Documentação (Português)

Documentação técnica da biblioteca **libdecaf** (Decaf), elaborada como material de apoio ao **Trabalho de Conclusão de Curso (TCC)** da Universidade de Brasília (UNB).

Complementa o [README.md](../../README.md) original do projeto (Mike Hamburg / Cryptography Research).

---

## Sobre este material

Este conjunto de documentos descreve:

- O **propósito** e os **padrões criptográficos** suportados (X25519, X448, Ed25519, Ed448);
- A **arquitetura em camadas** do código-fonte e o fluxo de **geração automática**;
- A **API pública** em C e os wrappers C++;
- Os fundamentos **Decaf/Ristretto** de eliminação de cofator;
- Aspectos de **segurança**, **testes** e **compilação**.

A biblioteca é de uso avançado: pressupõe familiaridade com criptografia de curvas elípticas. Não substitui orientação sobre desenho seguro de protocolos.

---

## Sumário

| # | Documento | Conteúdo |
|---|-----------|----------|
| 1 | [Visão geral](01-visao-geral.md) | Objetivos, padrões, metadados do projeto |
| 2 | [Arquitetura](02-arquitetura.md) | Camadas, diagramas, codificação Ristretto |
| 3 | [Estrutura do projeto](03-estrutura-do-projeto.md) | Diretórios, cabeçalhos gerados |
| 4 | [Compilação e geração de código](04-compilacao-e-geracao.md) | CMake, Make, `template.py`, tabelas |
| 5 | [API pública](05-api.md) | Cabeçalhos C/C++, operações, EdDSA |
| 6 | [Criptografia](06-criptografia.md) | Decaf/Ristretto, Elligator, multiplicação escalar |
| 7 | [Segurança e testes](07-seguranca-e-testes.md) | Tempo constante, testes, Python, licença |
| 8 | [Início rápido](08-inicio-rapido.md) | Exemplo mínimo em C |

**English:** [../en/README.md](../en/README.md)

---

## Referência rápida

| Atributo | Valor |
|----------|--------|
| Nome | DECAF (CMake) / libdecaf |
| Versão | 1.0 ([HISTORY.txt](../../HISTORY.txt)) |
| Autor principal | Mike Hamburg |
| Licença | MIT ([LICENSE.txt](../../LICENSE.txt)) |
| Linguagem | C99 + wrappers C++11 (somente cabeçalhos) |
| Curvas | Curve25519 / Ristretto, Ed448-Goldilocks |

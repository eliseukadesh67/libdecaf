# Visão geral

A **libdecaf** é uma biblioteca em C/C++ para pesquisa e uso em produção de curvas elípticas de Edwards torcidas (*twisted Edwards*). Ela expõe um grupo de **ordem prima** (técnica Decaf/Ristretto), os protocolos padronizados pelo CFRG e aritmética de baixo nível sobre as curvas suportadas.

| Atributo | Valor |
|----------|--------|
| **Nome do projeto** | DECAF (CMake) / libdecaf |
| **Versão** | 1.0 (ver [HISTORY.txt](../../HISTORY.txt)) |
| **Autor principal** | Mike Hamburg (Cryptography Research / Rambus) |
| **Licença** | MIT ([LICENSE.txt](../../LICENSE.txt)) |
| **Linguagem** | Biblioteca C99 + wrappers C++11 (somente cabeçalhos) |
| **Curvas** | Curve25519 / Ristretto (família Ed25519), Ed448-Goldilocks |

A biblioteca destina-se a desenvolvedores com conhecimento prévio de criptografia de curvas elípticas. **Não** ensina como projetar protocolos seguros.

## Objetivos e padrões suportados

### Objetivos principais

- **X25519 / X448** — troca de chaves Diffie–Hellman (RFC 7748)
- **Ed25519 / EdDSA-Ed448** — assinaturas digitais (RFC 8032)
- **Kit geral de EC** — pontos, escalares, pré-computação, *hash-to-curve* via Elligator, blocos para protocolos avançados (PAKE, VRF, etc.)

### O que a biblioteca deliberadamente não faz

- Expor informação de cofator (cofatores 4 ou 8 são removidos na API)
- Implementar criptografia simétrica completa (apenas SHA-512, SHA-3 e SHAKE exigidos pelos padrões)
- Garantir resistência a canais laterais além de tempo (ex.: análise de potência)

[Próximo: Arquitetura →](02-arquitetura.md)

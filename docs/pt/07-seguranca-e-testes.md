# Segurança e testes

## Disciplina de tempo constante

Intenção documentada (README e cabeçalhos gerados):

- Dados secretos não devem controlar **índices de vetores** nem **desvios condicionais** (exceto verificação final de sucesso/falha).
- Implementação em `constant_time.h` — `constant_time_lookup`, `cond_swap`, etc., amigáveis a vetorização.
- **Exceção:** `decaf_*_base_double_scalarmul_non_secret` — explicitamente **não** em tempo constante (apenas escalares públicos).

## Higiene de memória

- `secure_buffer` em C++ limpa dados na destruição
- `scalar_destroy` / `point_destroy` zeram estruturas

## Ressalvas

- **Sem garantia** de segurança; código sensível exige auditoria
- Mitigações a ataques de tempo; **análise de potência não** tratada
- Tratamento de falha do RNG listado como pendência ([TODO.txt](../../TODO.txt))

## Testes

### CMake / CTest (`test/`)

| Alvo | Finalidade |
|------|------------|
| `decaf_tester` | Testes funcionais principais (`test_decaf.cxx`) |
| `ristretto_tester` | Vetores Ristretto (`ristretto.cxx`) |
| `shakesum_tester` | Vetores SHAKE |
| `bench` | Benchmarks de desempenho |

Execução: `make test` ou `ctest` no diretório de build.

### Verificações adicionais

| Comando | Descrição |
|---------|-----------|
| `make test_ct` | Valgrind no teste de tempo constante (`test_ct.cxx`) |
| `make sagetest` | Testes algébricos com Sage (`test_decaf.sage`) |
| `make microbench` | Micro-benchmarks |

Vetores em: `test/vectors.inc.cxx`.

## Bindings Python

Pacote **`edgold`** em `python/`:

- Wrapper **ctypes** sobre `libdecaf.so`
- Módulo principal: `python/edgold/ed448.py` (Ed448-Goldilocks)
- Build: `python/setup.py` chama `gmake lib` no diretório pai
- Status: alfa

Opcional para consumidores C/C++; útil para scripts e experimentação.

## Licença e histórico

- **Licença:** MIT — núcleo Copyright 2014–2017 Cryptography Research, Inc. (Rambus); alguns arquivos Python sob BSD
- **Marcos** ([HISTORY.txt](../../HISTORY.txt)):
  - **1.0** (jul/2018): integração CMake (Johan Pascal)
  - Codificação **Ristretto** unificada (out/2017)
  - Sistema de templates multi-curva (2016)
  - STROBE removido antes do 1.0 (projeto separado)

## Referências bibliográficas e normativas

| Tema | Link |
|------|------|
| Técnica Decaf (cofator) | https://eprint.iacr.org/2015/673 |
| Codificação Ristretto | https://ristretto.group |
| RFC 7748 (X25519/X448) | https://www.rfc-editor.org/rfc/rfc7748 |
| RFC 8032 (EdDSA) | https://www.rfc-editor.org/rfc/rfc8032 |
| README upstream | [README.md](../../README.md) |

[← Criptografia](06-criptografia.md) · [Próximo: Início rápido →](08-inicio-rapido.md)

#!/usr/bin/env bash
#
# Reproducible evaluation of the EdDSA E-521 implementation
# (TCC - Eliseu Kadesh, UnB/FCTE).
#
# Builds libdecaf with the E-521 target, generates the base-point tables, and
# runs both validation suites (reference Ed521 test vectors + curve correctness
# and round-trips). Does not depend on any hard-coded path.
#
# Usage:
#   ./run_e521_tests.sh              compact summary (default)
#   ./run_e521_tests.sh --verbose    detailed output: hex of the vectors, the
#                                    produced keys/signatures, and [k]*G encodings
# Requirements: cmake >= 3.5, gcc/clang, make.
#
set -euo pipefail

VERBOSE=""
for arg in "$@"; do
    case "$arg" in
        -v|--verbose) VERBOSE="--verbose" ;;
        -h|--help) sed -n '2,14p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "unknown option: $arg (try --verbose or --help)" >&2; exit 2 ;;
    esac
done

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD="${ROOT}/build-e521"

echo "==> [1/4] Configuring (cmake) in ${BUILD}"
mkdir -p "${BUILD}"
cd "${BUILD}"
cmake -DENABLE_EXPERIMENTAL_E521=ON -DCMAKE_BUILD_TYPE=Release "${ROOT}" >/dev/null

echo "==> [2/4] Building the library and generating the base-point tables"
make -s decaf-static
make -s decaf_tables_e521
make -s decaf-static          # relink with the real tables

INC="${BUILD}/src/GENERATED/include"
LIB="${BUILD}/src/libdecaf.a"

echo "==> [3/4] Compiling the validation tests"
gcc -O2 "${ROOT}/test/test_ed521_vectors.c" -I"${ROOT}/test" -I"${INC}" "${LIB}" -o "${BUILD}/test_ed521_vectors"
gcc -O2 "${ROOT}/test/test_ed521.c"  -I"${ROOT}/test" -I"${INC}" "${LIB}" -o "${BUILD}/test_ed521"

echo "==> [4/4] Running the tests"
echo
"${BUILD}/test_ed521_vectors" ${VERBOSE}
echo
"${BUILD}/test_ed521" ${VERBOSE}
echo
echo "==> Done. If both suites reported success, the EdDSA E-521 build is validated."

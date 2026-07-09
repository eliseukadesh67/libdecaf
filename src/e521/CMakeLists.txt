############################################################################
# CMakeLists.txt
#
#    Released under the MIT License.  See LICENSE.txt for license information.
#
############################################################################

include_directories(
	${PROJECT_SOURCE_DIR}/src/p521
	${GSOURCE_PATH}/c/p521
	${PROJECT_SOURCE_DIR}/src/p521/${TARGET_ARCH_DIR_P521}
)

# NOTE: elligator.c (Ristretto hash-to-curve) is intentionally excluded for
# E-521.  The decaf elligator/invert map is unfinished upstream for the p521
# field (521 == 8*SER_BYTES + 1 needs a high-bit sign adjustment) and is not
# used by the EdDSA protocol, which is the scope of this work.  EdDSA relies
# only on decaf.c (encode/decode_like_eddsa), scalar.c and the base tables.
set(CURVE521_SOURCE_FILES_C
	${GSOURCE_PATH}/c/e521/decaf.c
	${GSOURCE_PATH}/c/e521/scalar.c
	${GSOURCE_PATH}/c/e521/eddsa.c
	${PROJECT_SOURCE_DIR}/src/e521/decaf_tables.c
)

SET_SOURCE_FILES_PROPERTIES(${GSOURCE_PATH}/c/e521/decaf.c PROPERTIES GENERATED 1)
SET_SOURCE_FILES_PROPERTIES(${GSOURCE_PATH}/c/e521/elligator.c PROPERTIES GENERATED 1)
SET_SOURCE_FILES_PROPERTIES(${GSOURCE_PATH}/c/e521/scalar.c PROPERTIES GENERATED 1)
SET_SOURCE_FILES_PROPERTIES(${GSOURCE_PATH}/c/e521/eddsa.c PROPERTIES GENERATED 1)
SET_SOURCE_FILES_PROPERTIES(${PROJECT_SOURCE_DIR}/src/e521/decaf_tables.c PROPERTIES GENERATED 1)

add_library(CURVE521 OBJECT ${CURVE521_SOURCE_FILES_C})
add_dependencies(CURVE521 generatedCode p521)
set_target_properties(CURVE521 PROPERTIES POSITION_INDEPENDENT_CODE True)

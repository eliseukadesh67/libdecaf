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

set(P521_HEADER_FILES
	${GSOURCE_PATH}/c/p521/f_field.h
	${TARGET_ARCH_DIR_P521}/f_impl.h
)
set(P521_SOURCE_FILES_C
	${TARGET_ARCH_DIR_P521}/f_impl.c
	f_arithmetic.c
	${GSOURCE_PATH}/c/p521/f_generic.c
)

SET_SOURCE_FILES_PROPERTIES(${GSOURCE_PATH}/c/p521/f_field.h PROPERTIES GENERATED 1)
SET_SOURCE_FILES_PROPERTIES(${GSOURCE_PATH}/c/p521/f_generic.c PROPERTIES GENERATED 1)

add_library(p521 OBJECT ${P521_HEADER_FILES} ${P521_SOURCE_FILES_C})
add_dependencies(p521 generatorP521)

set_target_properties(p521 PROPERTIES POSITION_INDEPENDENT_CODE True)

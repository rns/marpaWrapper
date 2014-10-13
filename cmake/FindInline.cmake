# Inspired from /usr/share/autoconf/autoconf/c.m4
#
# Note: __forceinline also exist, but is not recommended if the C compiler
#       think it is not worth inlining
#
FOREACH(KEYWORD "inline" "__inline__" "inline__" "__inline")
   IF(NOT DEFINED C_INLINE)
     TRY_COMPILE(C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
       ${CMAKE_CURRENT_SOURCE_DIR}/cmake/inline.c
       COMPILE_DEFINITIONS "-DC_INLINE=${KEYWORD}")
     IF(C_HAS_${KEYWORD})
       SET(C_INLINE TRUE)
       ADD_DEFINITIONS("-DC_INLINE=${KEYWORD}")
     ENDIF(C_HAS_${KEYWORD})
   ENDIF(NOT DEFINED C_INLINE)
ENDFOREACH(KEYWORD)
IF(NOT DEFINED C_INLINE)
   ADD_DEFINITIONS("-DC_INLINE=")
ENDIF(NOT DEFINED C_INLINE)

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (marpaWrapper)
#
# Module search path
#
SET (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
# ------------------------------
# Dependencies: bundled libmarpa
# ------------------------------
SET (MARPA_DIR "${PROJECT_SOURCE_DIR}/libmarpa-dist")
IF (NOT EXISTS "${MARPA_DIR}" OR NOT IS_DIRECTORY "${MARPA_DIR}")
  FIND_PROGRAM (PERL perl)
  MESSAGE (STATUS "Preparing ${MARPA_DIR}")
  GET_FILENAME_COMPONENT (CC_BASENAME ${CMAKE_C_COMPILER} NAME)
  EXECUTE_PROCESS (
    COMMAND make -f Makefile_For_Libmarpa.Unix
    )
  EXECUTE_PROCESS (
    COMMAND "${PERL}" win32/do_config_h.pl --use_perl_autoconf --cc "${CC_BASENAME}" --ccflags "${CMAKE_C_FLAGS}" --obj_ext "${CMAKE_C_OUTPUT_EXTENSION}"
    WORKING_DIRECTORY "${MARPA_DIR}"
    )
ENDIF ()

# --------
# config.h
# --------
FIND_PACKAGE(Inline)
FIND_PACKAGE(Va_copy)
FIND_PACKAGE(NullIsZeroes)
FIND_PACKAGE(Localtime_r)
INCLUDE(CheckIncludeFile)
CHECK_INCLUDE_FILE("inttypes.h" HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE("sys/time.h" HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE("sys/types.h" HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE("sys/socket.h" HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILE("sys/stat.h" HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE("stdint.h" HAVE_STDINT_H)
CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
CHECK_INCLUDE_FILE("fcntl.h" HAVE_FCNTL_H)
CHECK_INCLUDE_FILE("features.h" HAVE_FEATURES_H)
CHECK_INCLUDE_FILE("limits.h" HAVE_LIMITS_H)
CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
CHECK_INCLUDE_FILE("io.h" HAVE_IO_H)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)

INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include ${LIBMARPA_INCLUDE_DIR})
SET (CMAKE_SHARED_LINKER_FLAGS ${LIBMARPA_LIBRARIES})
# ---------------------------
# General overwrite of CFLAGS
# ---------------------------
IF (WIN32)
  # Loosely based on http://www.hdfgroup.org/ftp/HDF/HDF_Current/src/unpacked/config/cmake/ConfigureChecks.cmake
  ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")
  ADD_DEFINITIONS("-D_CRT_NONSTDC_NO_DEPRECATE")
ENDIF (WIN32)

IF(CMAKE_BUILD_TYPE MATCHES Release)
  SET (CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -DMARPAWRAPPER_NTRACE")
ENDIF(CMAKE_BUILD_TYPE MATCHES Release)

# Specific to GCC like family
IF (CMAKE_COMPILER_IS_GNUCC)
  IF (${CMAKE_C_FLAGS} MATCHES "std=c99")
    #
    # For functions like strdup that are not anymore in ISO C99 namespace
    ADD_DEFINITIONS("-D_GNU_SOURCE")
  ENDIF ()
ENDIF ()

# -------------------------------------------------
# General output path for libraries and executables
# -------------------------------------------------
SET (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)
SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)

# ------------------------
# marpaWrapper library
# ------------------------
ADD_LIBRARY (marpaWrapper SHARED
  src/marpaWrapper.c
  src/genericStack.c
  src/messageBuilder.c
  src/manageBuf.c
  src/dateBuilder.c
  src/genericLogger.c
  )
SET_PROPERTY (TARGET marpaWrapper APPEND PROPERTY INCLUDE_DIRECTORIES "${MARPA_INCLUDE_DIR}" "${PROJECT_SOURCE_DIR}")
SET (MYLIB_VERSION_MAJOR 0)
SET (MYLIB_VERSION_MINOR 1)
SET (MYLIB_VERSION_PATCH 1)
SET (MYLIB_VERSION_STRING ${MYLIB_VERSION_MAJOR}.${MYLIB_VERSION_MINOR}.${MYLIB_VERSION_PATCH})
SET_TARGET_PROPERTIES(marpaWrapper PROPERTIES VERSION ${MYLIB_VERSION_STRING} SOVERSION ${MYLIB_VERSION_MAJOR})


# -----------
# Executables
# -----------
ADD_EXECUTABLE        (marpaWrapper_test ${PROJECT_SOURCE_DIR}/test/marpaWrapper_test.c)
TARGET_LINK_LIBRARIES (marpaWrapper_test marpaWrapper)


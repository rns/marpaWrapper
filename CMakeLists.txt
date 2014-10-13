# ----------------------------------------------------------------
# Dependency: libmarpa (https://github.com/jeffreykegler/libmarpa)
#
# Quote from the author:
#
# Use the "dist" target at the top of Libmarpa to make an autoconf distribution in the dist/ directory.
# cd into that directory.
# ./configure
# make
# The .so will be in dist/.libs
#
# ./configure --enable-shared --disable-static --prefix=/tmp
# make
# make install
# ----------------------------------------------------------------
#
# Usage: On Windows: cmake -G "NMake Makefiles", then nmake install
#        On UNIX   : cmake -G "UNIX Makefiles", then make install
#
cmake_minimum_required (VERSION 2.8)
#
# As per http://www.cmake.org/pipermail/cmake/2008-September/023808.html
#
#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
#
IF (DEFINED CMAKE_BUILD_TYPE)
   SET (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ELSE ()
   SET (CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF ()

PROJECT (marpaWrapper)

SET (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
FIND_PACKAGE(Inline)
FIND_PACKAGE(Libmarpa)
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

# ----------------------
# va_copy implementation
# ----------------------
INCLUDE(VA_COPY)
VA_COPY()

# -------------------------
# prefer memset() over loop
# -------------------------
INCLUDE(NULL_IS_ZEROES)
NULL_IS_ZEROES()

# -------------
# localtime_r ?
# -------------
INCLUDE(LOCALTIME_R)
LOCALTIME_R()

# -------------------------
# Additional include checks
# -------------------------
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
INCLUDE(CheckTypeSize)
CHECK_TYPE_SIZE ("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)
CHECK_TYPE_SIZE ("unsigned int" SIZEOF_UINT BUILTIN_TYPES_ONLY)
CHECK_TYPE_SIZE ("unsigned long long int" SIZEOF_ULONGLONGINT BUILTIN_TYPES_ONLY)

# ------------------------------------------
# Generate marpaWrapper internal's config.h
# -----------------------------------------
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)

# -----------
# Executables
# -----------
ADD_EXECUTABLE        (marpaWrapper_test ${PROJECT_SOURCE_DIR}/test/marpaWrapper_test.c)
TARGET_LINK_LIBRARIES (marpaWrapper_test marpaWrapper)


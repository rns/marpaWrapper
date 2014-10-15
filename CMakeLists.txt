CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (marpaWrapper)

# ------------------
# Module search path
# ------------------
SET (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# ------------------
# Build dependencies
# ------------------
FIND_PROGRAM (PERL perl)

# ------------------------------
# Dependencies: bundled libmarpa
# ------------------------------
SET (MARPA_DIR "${PROJECT_SOURCE_DIR}/libmarpa-dist")
FILE(GLOB marpa_lib_src libmarpa-dist/*.c)
GET_FILENAME_COMPONENT (CC_BASENAME "${CMAKE_C_COMPILER}" NAME)
ADD_CUSTOM_COMMAND (
  PRE_BUILD
  COMMENT "Configuring libmarpa"
  OUTPUT libmarpa-dist/config.h
  COMMAND "${PERL}"
  ARGS win32/do_config_h.pl --use_perl_autoconf --cc "${CC_BASENAME}" --ccflags "${CMAKE_C_FLAGS}" --obj_ext "${CMAKE_C_OUTPUT_EXTENSION}"
  WORKING_DIRECTORY "${MARPA_DIR}"
  )

# --------
# config.h
# --------
FIND_PACKAGE(Inline)
FIND_PACKAGE(Va_copy)
FIND_PACKAGE(NullIsZeroes)
FIND_PACKAGE(Localtime_r)
INCLUDE(CheckIncludeFile)
CHECK_INCLUDE_FILE("sys/types.h" HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE("sys/time.h" HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
CHECK_INCLUDE_FILE("io.h" HAVE_IO_H)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/config.h")

INCLUDE_DIRECTORIES ("${PROJECT_SOURCE_DIR}/include" "${MARPA_DIR}")

# ---------------------------
# General overwrite of CFLAGS
# ---------------------------
IF (WIN32)
  # Loosely based on http://www.hdfgroup.org/ftp/HDF/HDF_Current/src/unpacked/config/cmake/ConfigureChecks.cmake
  ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")
  ADD_DEFINITIONS("-D_CRT_NONSTDC_NO_DEPRECATE")
ENDIF (WIN32)

IF(CMAKE_BUILD_TYPE MATCHES Release)
  SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMARPAWRAPPER_NTRACE")
ENDIF(CMAKE_BUILD_TYPE MATCHES Release)

# Specific to GCC like family
IF (CMAKE_COMPILER_IS_GNUCC)
  IF (${CMAKE_C_FLAGS} MATCHES "std=c99")
    # For functions like strdup that are not anymore in ISO C99 namespace
    ADD_DEFINITIONS("-D_GNU_SOURCE")
  ENDIF ()
ENDIF ()

# -------------------------------------------------
# General output path for libraries and executables
# -------------------------------------------------
SET (LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/build")
SET (EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/build")

# ------------------------
# marpaWrapper library
# ------------------------
FILE(GLOB marpaWrapper_lib_src src/*.c)
INCLUDE (GenerateExportHeader)
ADD_COMPILER_EXPORT_FLAGS ()
ADD_LIBRARY (marpaWrapper SHARED
  ${marpaWrapper_lib_src}
  ${marpa_lib_src}
  libmarpa-dist/config.h
  )
SET (MARPAWRAPPER_VERSION_MAJOR 0)
SET (MARPAWRAPPER_VERSION_MINOR 1)
SET (MARPAWRAPPER_VERSION_PATCH 1)
SET (MARPAWRAPPER_VERSION ${MARPAWRAPPER_VERSION_MAJOR}.${MARPAWRAPPER_VERSION_MINOR}.${MARPAWRAPPER_VERSION_PATCH})
SET_TARGET_PROPERTIES(marpaWrapper PROPERTIES VERSION ${MARPAWRAPPER_VERSION} SOVERSION ${MARPAWRAPPER_VERSION_MAJOR})
GENERATE_EXPORT_HEADER(marpaWrapper
  BASE_NAME MARPAWRAPPER_01
  EXPORT_MACRO_NAME EXPORT
  EXPORT_FILE_NAME include/marpaWrapper_Export.h
  DEPRECATED_MACRO_NAME DEPRECATED
  NO_EXPORT_MACRO_NAME NO_EXPORT
  STATIC_DEFINE STATIC
  NO_DEPRECATED_MACRO_NAME NO_DEPRECATED
  DEFINE_NO_DEPRECATED
  PREFIX_NAME MARPAWRAPPER_
)
# -----------------------
# distclean custom target
# -----------------------
ADD_CUSTOM_TARGET (distclean)
SET(DISTCLEANED
  build
  CMakeFiles
  CMakeCache.txt
  Makefile
  cmake_install.cmake
  "${MARPA_DIR}/config.log"
  "${MARPA_DIR}/config.h"
  "${MARPA_DIR}/config_from_autoconf.h"
  "${MARPA_DIR}/stamp-h1"
  include/config.h
  GLOB *vcxproj*
  GLOB *sln
  GLOB *suo
  )
ADD_CUSTOM_COMMAND (
  DEPENDS clean
  COMMENT "distclean"
  COMMAND "${PERL}"
  ARGS    -MExtUtils::Command -e rm_rf ${DISTCLEANED}
  TARGET  distclean
  )

# -----------
# Executables
# -----------
ADD_EXECUTABLE        (marpaWrapper_test ${PROJECT_SOURCE_DIR}/test/marpaWrapper_test.c)
TARGET_LINK_LIBRARIES (marpaWrapper_test marpaWrapper)

# -------
# Install
# -------
# Loosely based on https://code.google.com/p/bullet/source/browse/trunk/CMakeLists.txt
IF ("${CMAKE_GENERATOR}" MATCHES "Unix Makefiles")
  OPTION (INSTALL_LIBS "Set when you want to install libraries" ON)
ELSE ()
  IF (APPLE AND FRAMEWORK)
    OPTION (INSTALL_LIBS "Set when you want to install libraries" ON)
  ELSE ()
    #by default, don't enable the 'INSTALL' option for Xcode and MSVC projectfiles
    OPTION (INSTALL_LIBS "Set when you want to install libraries" OFF)
  ENDIF ()
ENDIF ()

IF (INSTALL_LIBS)
 SET (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
 SET (LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING "Library directory name")
 ## the following are directories where stuff will be installed to
 SET (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include/" CACHE PATH "The subdirectory to the header prefix")
 SET (PKGCONFIG_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/pkgconfig/" CACHE STRING "Base directory for pkgconfig files")
 IF (NOT WIN32)
   CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/marpaWrapper.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/marpaWrapper.pc @ONLY)
   INSTALL(
     FILES
     ${CMAKE_CURRENT_BINARY_DIR}/marpaWrapper.pc
     DESTINATION
     ${PKGCONFIG_INSTALL_PREFIX})
 ENDIF(NOT WIN32)
ENDIF(INSTALL_LIBS)

#INSTALL of other files requires CMake 2.6
IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)
  OPTION (INSTALL_EXTRA_LIBS "Set when you want extra libraries installed" OFF)
ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)

# As per https://www.semipol.de/archives/251
MACRO(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST)
  FOREACH(HEADER ${${HEADER_LIST}})
    STRING(REGEX MATCH "(.*)[/\\]" DIR ${HEADER})
    INSTALL(FILES ${HEADER} DESTINATION include/${DIR})
  ENDFOREACH(HEADER)
ENDMACRO(INSTALL_HEADERS_WITH_DIRECTORY)

SET(HEADERS include/marpaWrapper.h include/marpaWrapper_Export.h include/genericLogger.h)
INSTALL_HEADERS_WITH_DIRECTORY(HEADERS)
INSTALL(TARGETS marpaWrapper DESTINATION "lib")

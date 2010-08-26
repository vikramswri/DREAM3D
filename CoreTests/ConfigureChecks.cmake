
 
# In this file we are doing all of our 'configure' checks. Things like checking
# for headers, functions, libraries, types and size of types.
INCLUDE (${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/CheckTypeSize.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/CheckCXXSourceCompiles.cmake)


INCLUDE (${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(MXA_WORDS_BIGENDIAN)

  
IF(WIN32)
  IF(NOT UNIX)
    SET(${PROJECT_PREFIX}_HAVE_WINDOWS 1)
    SET(${PROJECT_PREFIX}_HAVE_WINSOCK_H 1)
  ENDIF(NOT UNIX)
ENDIF(WIN32)

#-----------------------------------------------------------------------------
#  Check the size in bytes of all the int and float types
#-----------------------------------------------------------------------------
MACRO ( CORE_CHECK_TYPE_SIZE type var)
    SET (aType ${type})
    SET (aVar  ${var})
    CHECK_TYPE_SIZE(${aType}   ${aVar})
    IF (NOT ${aVar})
        SET (${aVar} 0 CACHE INTERNAL "SizeOf for ${aType}")
    ENDIF()
ENDMACRO()

CORE_CHECK_TYPE_SIZE(char           ${PROJECT_PREFIX}_SIZEOF_CHAR)
CORE_CHECK_TYPE_SIZE(short          ${PROJECT_PREFIX}_SIZEOF_SHORT)
CORE_CHECK_TYPE_SIZE(int            ${PROJECT_PREFIX}_SIZEOF_INT)
CORE_CHECK_TYPE_SIZE(unsigned       ${PROJECT_PREFIX}_SIZEOF_UNSIGNED)
IF (NOT APPLE)
    CORE_CHECK_TYPE_SIZE(long           ${PROJECT_PREFIX}_SIZEOF_LONG)
ENDIF()
CORE_CHECK_TYPE_SIZE("long long"    ${PROJECT_PREFIX}_SIZEOF_LONG_LONG)
CORE_CHECK_TYPE_SIZE(__int64        ${PROJECT_PREFIX}_SIZEOF___INT64)
IF (NOT ${PROJECT_PREFIX}_SIZEOF___INT64)
    SET (${PROJECT_PREFIX}_SIZEOF___INT64 0)
ENDIF()

CORE_CHECK_TYPE_SIZE(float          ${PROJECT_PREFIX}_SIZEOF_FLOAT)
CORE_CHECK_TYPE_SIZE(double         ${PROJECT_PREFIX}_SIZEOF_DOUBLE)
CORE_CHECK_TYPE_SIZE("long double"  ${PROJECT_PREFIX}_SIZEOF_LONG_DOUBLE)
CORE_CHECK_TYPE_SIZE(int8_t         ${PROJECT_PREFIX}_SIZEOF_INT8_T)
CORE_CHECK_TYPE_SIZE(uint8_t        ${PROJECT_PREFIX}_SIZEOF_UINT8_T)
CORE_CHECK_TYPE_SIZE(int_least8_t   ${PROJECT_PREFIX}_SIZEOF_INT_LEAST8_T)
CORE_CHECK_TYPE_SIZE(uint_least8_t  ${PROJECT_PREFIX}_SIZEOF_UINT_LEAST8_T)
CORE_CHECK_TYPE_SIZE(int_fast8_t    ${PROJECT_PREFIX}_SIZEOF_INT_FAST8_T)
CORE_CHECK_TYPE_SIZE(uint_fast8_t   ${PROJECT_PREFIX}_SIZEOF_UINT_FAST8_T)
CORE_CHECK_TYPE_SIZE(int16_t        ${PROJECT_PREFIX}_SIZEOF_INT16_T)
CORE_CHECK_TYPE_SIZE(uint16_t       ${PROJECT_PREFIX}_SIZEOF_UINT16_T)
CORE_CHECK_TYPE_SIZE(int_least16_t  ${PROJECT_PREFIX}_SIZEOF_INT_LEAST16_T)
CORE_CHECK_TYPE_SIZE(uint_least16_t ${PROJECT_PREFIX}_SIZEOF_UINT_LEAST16_T)
CORE_CHECK_TYPE_SIZE(int_fast16_t   ${PROJECT_PREFIX}_SIZEOF_INT_FAST16_T)
CORE_CHECK_TYPE_SIZE(uint_fast16_t  ${PROJECT_PREFIX}_SIZEOF_UINT_FAST16_T)
CORE_CHECK_TYPE_SIZE(int32_t        ${PROJECT_PREFIX}_SIZEOF_INT32_T)
CORE_CHECK_TYPE_SIZE(uint32_t       ${PROJECT_PREFIX}_SIZEOF_UINT32_T)
CORE_CHECK_TYPE_SIZE(int_least32_t  ${PROJECT_PREFIX}_SIZEOF_INT_LEAST32_T)
CORE_CHECK_TYPE_SIZE(uint_least32_t ${PROJECT_PREFIX}_SIZEOF_UINT_LEAST32_T)
CORE_CHECK_TYPE_SIZE(int_fast32_t   ${PROJECT_PREFIX}_SIZEOF_INT_FAST32_T)
CORE_CHECK_TYPE_SIZE(uint_fast32_t  ${PROJECT_PREFIX}_SIZEOF_UINT_FAST32_T)
CORE_CHECK_TYPE_SIZE(int64_t        ${PROJECT_PREFIX}_SIZEOF_INT64_T)
CORE_CHECK_TYPE_SIZE(uint64_t       ${PROJECT_PREFIX}_SIZEOF_UINT64_T)
CORE_CHECK_TYPE_SIZE(int_least64_t  ${PROJECT_PREFIX}_SIZEOF_INT_LEAST64_T)
CORE_CHECK_TYPE_SIZE(uint_least64_t ${PROJECT_PREFIX}_SIZEOF_UINT_LEAST64_T)
CORE_CHECK_TYPE_SIZE(int_fast64_t   ${PROJECT_PREFIX}_SIZEOF_INT_FAST64_T)
CORE_CHECK_TYPE_SIZE(uint_fast64_t  ${PROJECT_PREFIX}_SIZEOF_UINT_FAST64_T)
IF (NOT APPLE)
    CORE_CHECK_TYPE_SIZE(size_t         ${PROJECT_PREFIX}_SIZEOF_SIZE_T)
    CORE_CHECK_TYPE_SIZE(ssize_t        ${PROJECT_PREFIX}_SIZEOF_SSIZE_T)
    IF (NOT ${PROJECT_PREFIX}_SIZEOF_SSIZE_T)
        SET (${PROJECT_PREFIX}_SIZEOF_SSIZE_T 0)
    ENDIF()
ENDIF()
CORE_CHECK_TYPE_SIZE(off_t          ${PROJECT_PREFIX}_SIZEOF_OFF_T)
CORE_CHECK_TYPE_SIZE(off64_t        ${PROJECT_PREFIX}_SIZEOF_OFF64_T)
IF (NOT ${PROJECT_PREFIX}_SIZEOF_OFF64_T)
    SET (${PROJECT_PREFIX}_SIZEOF_OFF64_T 0)
ENDIF()


IF(${PROJECT_PREFIX}_SIZEOF___INT64)
  IF("${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64" MATCHES "^${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64$")
    MESSAGE(STATUS "Checking whether long and __int64 are the same type")
    TRY_COMPILE(${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64
      ${PROJECT_BINARY_DIR}/CMakeTmp
      ${PROJECT_RESOURCES_DIR}/CMake/TestCompareTypes.cxx
      COMPILE_DEFINITIONS
      -D${PROJECT_PREFIX}_TEST_COMPARE_TYPE_1=long
      -D${PROJECT_PREFIX}_TEST_COMPARE_TYPE_2=__int64
      OUTPUT_VARIABLE OUTPUT)
    IF(${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64)
      MESSAGE(STATUS "Checking whether long and __int64 are the same type -- yes")
      SET(${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64 1 CACHE INTERNAL "Whether long and __int64 are the same type")
      WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
        "Determining whether long and __int64 are the same type "
        "passed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    ELSE(${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64)
      MESSAGE(STATUS "Checking whether long and __int64 are the same type -- no")
      SET(${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64 0 CACHE INTERNAL "Whether long and __int64 are the same type")
      WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
        "Determining whether long and __int64 are the same type "
        "failed with the following output:\n"
        "${OUTPUT}\n" APPEND)
    ENDIF(${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64)
  ENDIF("${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64" MATCHES "^${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64$")
  IF(${PROJECT_PREFIX}_SIZEOF_LONG_LONG)
    IF("${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64" MATCHES "^${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64$")
      MESSAGE(STATUS "Checking whether long long and __int64 are the same type")
      TRY_COMPILE(${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64
        ${PROJECT_BINARY_DIR}/CMakeTmp
        ${PROJECT_RESOURCES_DIR}/CMake/TestCompareTypes.cxx
        COMPILE_DEFINITIONS
        -D${PROJECT_PREFIX}_TEST_COMPARE_TYPE_1=TYPE_LONG_LONG
        -D${PROJECT_PREFIX}_TEST_COMPARE_TYPE_2=__int64
        OUTPUT_VARIABLE OUTPUT)
      IF(${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64)
        MESSAGE(STATUS "Checking whether long long and __int64 are the same type -- yes")
        SET(${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64 1 CACHE INTERNAL "Whether long long and __int64 are the same type")
        WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
          "Determining whether long long and __int64 are the same type "
          "passed with the following output:\n"
          "${OUTPUT}\n" APPEND)
      ELSE(${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64)
        MESSAGE(STATUS "Checking whether long long and __int64 are the same type -- no")
        SET(${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64 0 CACHE INTERNAL "Whether long long and __int64 are the same type")
        WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
          "Determining whether long long and __int64 are the same type "
          "failed with the following output:\n"
          "${OUTPUT}\n" APPEND)
      ENDIF(${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64)
    ENDIF("${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64" MATCHES "^${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64$")
  ENDIF(${PROJECT_PREFIX}_SIZEOF_LONG_LONG)
  IF(NOT ${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64)
    IF(NOT ${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64)
#  VS 6 cannot convert unsigned __int64 to double unless the
# "Visual C++ Processor Pack" is installed.
      IF("${PROJECT_PREFIX}_TYPE_CONVERT_UI64_TO_DOUBLE" MATCHES "^${PROJECT_PREFIX}_TYPE_CONVERT_UI64_TO_DOUBLE$")
        MESSAGE(STATUS "Checking whether unsigned __int64 can convert to double")
        TRY_COMPILE(${PROJECT_PREFIX}_TYPE_CONVERT_UI64_TO_DOUBLE
          ${PROJECT_BINARY_DIR}/CMakeTmp
          ${PROJECT_RESOURCES_DIR}/CMake/TestConvertTypes.cxx
          COMPILE_DEFINITIONS
          -D${PROJECT_PREFIX}_TEST_CONVERT_TYPE_FROM=TYPE_UNSIGNED___INT64
          -D${PROJECT_PREFIX}_TEST_CONVERT_TYPE_TO=double
          OUTPUT_VARIABLE OUTPUT)
        IF(${PROJECT_PREFIX}_TYPE_CONVERT_UI64_TO_DOUBLE)
          MESSAGE(STATUS "Checking whether unsigned __int64 can convert to double -- yes")
          SET(${PROJECT_PREFIX}_TYPE_CONVERT_UI64_TO_DOUBLE 1 CACHE INTERNAL "Whether unsigned __int64 can convert to double")
          WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
            "Determining whether unsigned __int64 can convert to double "
            "passed with the following output:\n"
            "${OUTPUT}\n" APPEND)
        ELSE(${PROJECT_PREFIX}_TYPE_CONVERT_UI64_TO_DOUBLE)
          MESSAGE(STATUS "Checking whether unsigned __int64 can convert to double -- no")
          SET(${PROJECT_PREFIX}_TYPE_CONVERT_UI64_TO_DOUBLE 0 CACHE INTERNAL "Whether unsigned __int64 can convert to double")
          WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
            "Determining whether unsigned __int64 can convert to double "
            "failed with the following output:\n"
            "${OUTPUT}\n" APPEND)
        ENDIF(${PROJECT_PREFIX}_TYPE_CONVERT_UI64_TO_DOUBLE)
      ENDIF("${PROJECT_PREFIX}_TYPE_CONVERT_UI64_TO_DOUBLE" MATCHES "^${PROJECT_PREFIX}_TYPE_CONVERT_UI64_TO_DOUBLE$")
    ENDIF(NOT ${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64)
  ENDIF(NOT ${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64)
ENDIF(${PROJECT_PREFIX}_SIZEOF___INT64)

# Enable the "long long" type if it is available.  It is standard in
# C99 and C++03 but not in earlier standards.
SET(${PROJECT_PREFIX}_TYPE_USE_LONG_LONG)
IF(${PROJECT_PREFIX}_SIZEOF_LONG_LONG)
  SET(${PROJECT_PREFIX}_TYPE_USE_LONG_LONG 1)
ENDIF(${PROJECT_PREFIX}_SIZEOF_LONG_LONG)

# Enable the "__int64" type if it is available and unique.  It is not
# standard.
SET(${PROJECT_PREFIX}_TYPE_USE___INT64)
IF(${PROJECT_PREFIX}_SIZEOF___INT64)
  IF(NOT ${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64)
    IF(NOT ${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64)
      SET(${PROJECT_PREFIX}_TYPE_USE___INT64 1)
    ENDIF(NOT ${PROJECT_PREFIX}_TYPE_SAME_LONG_LONG_AND___INT64)
  ENDIF(NOT ${PROJECT_PREFIX}_TYPE_SAME_LONG_AND___INT64)
ENDIF(${PROJECT_PREFIX}_SIZEOF___INT64)

IF("${PROJECT_PREFIX}_COMPILER_HAS_BOOL" MATCHES "^${PROJECT_PREFIX}_COMPILER_HAS_BOOL$")
  MESSAGE(STATUS "Checking support for C++ type bool")
  TRY_COMPILE(${PROJECT_PREFIX}_COMPILER_HAS_BOOL
              ${PROJECT_BINARY_DIR}/CMakeTmp/Bool
              ${PROJECT_RESOURCES_DIR}/CMake/TestBoolType.cxx
              OUTPUT_VARIABLE OUTPUT)
  IF(${PROJECT_PREFIX}_COMPILER_HAS_BOOL)
    MESSAGE(STATUS "Checking support for C++ type bool -- yes")
    SET(${PROJECT_PREFIX}_COMPILER_HAS_BOOL 1 CACHE INTERNAL "Support for C++ type bool")
    WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
      "Determining if the C++ compiler supports type bool "
      "passed with the following output:\n"
      "${OUTPUT}\n" APPEND)
  ELSE(${PROJECT_PREFIX}_COMPILER_HAS_BOOL)
    MESSAGE(STATUS "Checking support for C++ type bool -- no")
    SET(${PROJECT_PREFIX}_COMPILER_HAS_BOOL 0 CACHE INTERNAL "Support for C++ type bool")
    WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
      "Determining if the C++ compiler supports type bool "
      "failed with the following output:\n"
      "${OUTPUT}\n" APPEND)
  ENDIF(${PROJECT_PREFIX}_COMPILER_HAS_BOOL)
ENDIF("${PROJECT_PREFIX}_COMPILER_HAS_BOOL" MATCHES "^${PROJECT_PREFIX}_COMPILER_HAS_BOOL$")

IF("${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED" MATCHES "^${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED$")
  MESSAGE(STATUS "Checking signedness of char")
  TRY_RUN(${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED ${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED_COMPILED
          ${PROJECT_BINARY_DIR}/CMakeTmp/Char
          ${PROJECT_RESOURCES_DIR}/CMake/TestCharSignedness.cxx)
  IF(${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED_COMPILED)
    IF(${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED)
      MESSAGE(STATUS "Checking signedness of char -- signed")
      SET(${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED 1 CACHE INTERNAL "Whether char is signed.")
    ELSE(${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED)
      MESSAGE(STATUS "Checking signedness of char -- unsigned")
      SET(${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED 0 CACHE INTERNAL "Whether char is signed.")
    ENDIF(${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED)
  ELSE(${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED_COMPILED)
    MESSAGE(STATUS "Checking signedness of char -- failed")
  ENDIF(${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED_COMPILED)
ENDIF("${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED" MATCHES "^${PROJECT_PREFIX}_TYPE_CHAR_IS_SIGNED$")


# --------------------------------------------------------------------
#  Now check for needed Header files
# --------------------------------------------------------------------
macro (CORE_CHECK_INCLUDE_FILE header var prefix)
    CHECK_INCLUDE_FILE("${header}"        ${prefix}_${var} )
endmacro()


CORE_CHECK_INCLUDE_FILE("stddef.h"        HAVE_STDDEF_H   MXA)
CORE_CHECK_INCLUDE_FILE("stdint.h"        HAVE_STDINT_H   MXA)
CORE_CHECK_INCLUDE_FILE("stdlib.h"        HAVE_STDLIB_H   MXA)
CORE_CHECK_INCLUDE_FILE("setjmp.h"        HAVE_SETJMP_H   MXA)
CORE_CHECK_INCLUDE_FILE("string.h"        HAVE_STRING_H   MXA)
CORE_CHECK_INCLUDE_FILE("stdio.h"         HAVE_STDIO_H   MXA)
CORE_CHECK_INCLUDE_FILE("math.h"          HAVE_MATH_H   MXA)
CORE_CHECK_INCLUDE_FILE("time.h"          HAVE_TIME_H   MXA)
CORE_CHECK_INCLUDE_FILE("sys/time.h"      HAVE_SYS_TIME_H   MXA)
CORE_CHECK_INCLUDE_FILE("sys/types.h"     HAVE_SYS_TYPES_H   MXA)
CORE_CHECK_INCLUDE_FILE("sys/socket.h"    HAVE_SYS_SOCKET_H   MXA)
CORE_CHECK_INCLUDE_FILE("sys/stat.h"      HAVE_SYS_STAT_H   MXA)
CORE_CHECK_INCLUDE_FILE("netinet/in.h"    HAVE_NETINET_IN_H   MXA)
CORE_CHECK_INCLUDE_FILE("arpa/inet.h"     HAVE_ARPA_INET_H   MXA)
CORE_CHECK_INCLUDE_FILE("unistd.h"        HAVE_UNISTD_H   MXA)
CORE_CHECK_INCLUDE_FILE("fcntl.h"         HAVE_FCNTL_H   MXA)
CORE_CHECK_INCLUDE_FILE("errno.h"         HAVE_ERRNO_H   MXA)

SET (VERSION_COMPILE_FLAGS "")

if (MSVC)
 set(${PROJECT_PREFIX}_HAVE_TIME_GETTIMEOFDAY "TRUE")
 set(VERSION_COMPILE_FLAGS "/D${PROJECT_PREFIX}_HAVE_TIME_GETTIMEOFDAY")
endif()
 
IF (NOT MSVC)
    TRY_COMPILE(${PROJECT_PREFIX}_HAVE_TIME_GETTIMEOFDAY
          ${CMAKE_BINARY_DIR}
          ${PROJECT_RESOURCES_DIR}/CMake/GetTimeOfDayTest.cpp
          COMPILE_DEFINITIONS -DTRY_TIME_H
          OUTPUT_VARIABLE OUTPUT)
    IF (${PROJECT_PREFIX}_HAVE_TIME_GETTIMEOFDAY STREQUAL "TRUE")
        SET (${PROJECT_PREFIX}_HAVE_TIME_GETTIMEOFDAY "1")
        set(VERSION_COMPILE_FLAGS "-D${PROJECT_PREFIX}_HAVE_TIME_GETTIMEOFDAY")
    ENDIF (${PROJECT_PREFIX}_HAVE_TIME_GETTIMEOFDAY STREQUAL "TRUE")

    TRY_COMPILE(${PROJECT_PREFIX}_HAVE_SYS_TIME_GETTIMEOFDAY
          ${CMAKE_BINARY_DIR}
          ${PROJECT_RESOURCES_DIR}/CMake/GetTimeOfDayTest.cpp
          COMPILE_DEFINITIONS -DTRY_SYS_TIME_H
          OUTPUT_VARIABLE OUTPUT)
    IF (${PROJECT_PREFIX}_HAVE_SYS_TIME_GETTIMEOFDAY STREQUAL "TRUE")
        SET (${PROJECT_PREFIX}_HAVE_SYS_TIME_GETTIMEOFDAY "1")
        set(VERSION_COMPILE_FLAGS "-D${PROJECT_PREFIX}_HAVE_SYS_TIME_GETTIMEOFDAY")
    ENDIF (${PROJECT_PREFIX}_HAVE_SYS_TIME_GETTIMEOFDAY STREQUAL "TRUE")

    if (NOT ${PROJECT_PREFIX}_HAVE_SYS_TIME_GETTIMEOFDAY AND NOT ${PROJECT_PREFIX}_HAVE_TIME_GETTIMEOFDAY)
        message(STATUS "---------------------------------------------------------------")
        message(STATUS "Function 'gettimeofday()' was not found. MXADataModel will use its")
        message(STATUS "  own implementation.. This can happen on older versions of")
        message(STATUS "  MinGW on Windows. Consider upgrading your MinGW installation")
        message(STATUS "  to a newer version such as MinGW 3.12")
        message(STATUS "---------------------------------------------------------------")
    endif()
ENDIF (NOT MSVC)

#-----------------------------------------------------------------------------
# Check how to print a Long Long integer
#-----------------------------------------------------------------------------
SET(MXA_MXA_PRINTF_LL_WIDTH "MXA_PRINTF_LL_WIDTH")
IF (MXA_PRINTF_LL_WIDTH MATCHES "^MXA_PRINTF_LL_WIDTH$")
  SET(PRINT_LL_FOUND 0)
  MESSAGE(STATUS "Checking for appropriate format for 64 bit long:")
  FOREACH(MXA_PRINTF_LL l64 l L q I64 ll)
    SET(CURRENT_TEST_DEFINITIONS "-DPRINTF_LL_WIDTH=${MXA_PRINTF_LL}")
    IF (MXA_SIZEOF_LONG_LONG)
      SET(CURRENT_TEST_DEFINITIONS "${CURRENT_TEST_DEFINITIONS} -DHAVE_LONG_LONG")
    ENDIF (MXA_SIZEOF_LONG_LONG)
    TRY_RUN(MXA_PRINTF_LL_TEST_RUN   MXA_PRINTF_LL_TEST_COMPILE
      ${MXADataModel_BINARY_DIR}/CMake
      ${PROJECT_RESOURCES_DIR}/CMake/TestMiscFeatures.c
      CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${CURRENT_TEST_DEFINITIONS}
      OUTPUT_VARIABLE OUTPUT)
    IF (MXA_PRINTF_LL_TEST_COMPILE)
      IF (MXA_PRINTF_LL_TEST_RUN MATCHES 0)
        SET(MXA_PRINTF_LL_WIDTH "\"${MXA_PRINTF_LL}\"" CACHE INTERNAL "Width for printf for type `long long' or `__int64', us. `ll")
        SET(PRINT_LL_FOUND 1)
      ELSE (MXA_PRINTF_LL_TEST_RUN MATCHES 0)
        MESSAGE("Width with ${MXA_PRINTF_LL} failed with result: ${MXA_PRINTF_LL_TEST_RUN}")
      ENDIF (MXA_PRINTF_LL_TEST_RUN MATCHES 0)
    ELSE (MXA_PRINTF_LL_TEST_COMPILE)
      FILE( APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
        "Test MXA_PRINTF_LL_WIDTH for ${MXA_PRINTF_LL} failed with the following output:\n ${OUTPUT}\n")
    ENDIF (MXA_PRINTF_LL_TEST_COMPILE)
  ENDFOREACH(MXA_PRINTF_LL)
  IF (PRINT_LL_FOUND)
    MESSAGE(STATUS "Checking for apropriate format for 64 bit long: found ${MXA_PRINTF_LL_WIDTH}")
  ELSE (PRINT_LL_FOUND)
    MESSAGE(STATUS "Checking for apropriate format for 64 bit long: not found")
    SET(MXA_PRINTF_LL_WIDTH "\"unknown\"" CACHE INTERNAL
      "Width for printf for type `long long' or `__int64', us. `ll")
  ENDIF (PRINT_LL_FOUND)
ENDIF (MXA_PRINTF_LL_WIDTH MATCHES "^MXA_PRINTF_LL_WIDTH$")


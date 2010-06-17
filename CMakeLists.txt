#///////////////////////////////////////////////////////////////////////////////
#
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#
#///////////////////////////////////////////////////////////////////////////////
project (EmMpmProj)
cmake_minimum_required(VERSION 2.6)

# ---------- Setup output Directories -------------------------
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/Bin
  CACHE PATH
  "Single Directory for all Libraries"
  )

# --------- Setup the Executable output Directory -------------
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/Bin
  CACHE PATH
  "Single Directory for all Executables."
  )

# --------- Setup the Executable output Directory -------------
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/Bin
  CACHE PATH
  "Single Directory for all static libraries."
  )

SET (EmMpmProj_PROJECT_DIR ${EmMpmProj_SOURCE_DIR})
SET (PROJECT_RESOURCES_DIR ${EmMpmProj_SOURCE_DIR}/Resources)
SET (EmMpmProj_CMAKE_DIR  ${PROJECT_RESOURCES_DIR}/CMake)
SET (OSX_TOOLS_DIR ${PROJECT_RESOURCES_DIR}/OSX_Tools)
SET (PROJECT_PREFIX "EmMpm" CACHE STRING "The 3 Letter Prefix to be used for Preprocessor definitions")
SET (EXE_DEBUG_EXTENSION "_debug")

INCLUDE (${PROJECT_RESOURCES_DIR}/CMake/CMakeMacros.cmake )
INCLUDE (${PROJECT_RESOURCES_DIR}/CMake/ConfigureChecks.cmake)

# --------------------------------------------------------------------
# Add in some compiler definitions
# --------------------------------------------------------------------
IF ( CMAKE_BUILD_TYPE MATCHES Debug )
  ADD_DEFINITIONS(-DDEBUG)
  IF (NOT WIN32)
     #Doing this with NMake files will overwhelm the output
    ADD_DEFINITIONS(-Wall)
  ENDIF()
ENDIF ( CMAKE_BUILD_TYPE MATCHES Debug )

# --------------------------------------------------------------------
# If was are using GCC, make the compiler messages on a single line
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF (MSVC)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

# Include some directories for the compiler
include_directories(${PROJECT_BINARY_DIR})

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
GenerateVersionString( ${PROJECT_NAME} "${EmMpmGUI_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Version.h" "${PROJECT_PREFIX}" )


# ------------------------------------------------------------------------------
#  Qt 4 Section
#------------------------------------------------------------------------------
# by default only QtCore and QtGui modules are enabled
# other modules must be enabled like this:  
IF (WIN32)
  SET (QT_USE_QTMAIN TRUE)
ENDIF (WIN32)

# this command finds Qt4 libraries and sets all required variables
# note that it's Qt4, not QT4 or qt4
FIND_PACKAGE( Qt4 REQUIRED )
# add some useful macros and variables
# (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that 
# contains a path to CMake script)
INCLUDE( ${QT_USE_FILE} )
   
# Add our Qt based GUI project to the list of targets
ADD_SUBDIRECTORY( ${EmMpmProj_SOURCE_DIR}/Source/EmMpm/GUI ${EmMpmProj_BINARY_DIR}/GUI)
   
# --------------------------------------------------------------------
# Generate our header files
# --------------------------------------------------------------------
configure_file(${PROJECT_RESOURCES_DIR}/CMake/Configuration.h.in
  ${EmMpmGUI_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Configuration.h )

configure_file(${PROJECT_RESOURCES_DIR}/CMake/PrimitiveTypes.h.in
               ${EmMpmGUI_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Types.h )

configure_file(${PROJECT_SOURCE_DIR}/License.txt.in
                ${PROJECT_BINARY_DIR}/License.txt )
configure_file(${PROJECT_SOURCE_DIR}/ReadMe.txt.in
                ${PROJECT_BINARY_DIR}/ReadMe.txt )
IDE_SOURCE_PROPERTIES( "EmMpm/Configured" "${EmMpmGUI_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Version.h;${EmMpmGUI_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Configuration.h;${EmMpmGUI_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Types.h" "")                

include(${PROJECT_RESOURCES_DIR}/CMake/InstallSupportLibraries.cmake)
include(${PROJECT_RESOURCES_DIR}/CMake/PackageProject.cmake)

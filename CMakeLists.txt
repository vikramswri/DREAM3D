#///////////////////////////////////////////////////////////////////////////////
#
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#
#///////////////////////////////////////////////////////////////////////////////
project (TO79)
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

SET (TO79_PROJECT_DIR ${TO79_SOURCE_DIR})
SET (PROJECT_RESOURCES_DIR ${TO79_SOURCE_DIR}/Resources)
SET (PROJECT_CMAKE_DIR  ${PROJECT_RESOURCES_DIR}/MXACMake)
SET (OSX_TOOLS_DIR ${PROJECT_RESOURCES_DIR}/OSX_Tools)
SET (PROJECT_PREFIX "TO79" CACHE STRING "The 3 Letter Prefix to be used for Preprocessor definitions")
SET (EXE_DEBUG_EXTENSION "_debug")

# --------------------------------------------------------------------
# including this file will kick off all the type, header and other configuration
# checks that need to be performed in order to include all the MXA Files
INCLUDE (${PROJECT_CMAKE_DIR}/MXACMakeCommon.cmake )


# Include some directories for the compiler
include_directories(${PROJECT_BINARY_DIR})

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
GenerateVersionString( ${PROJECT_NAME} "${TO79_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Version.h" "${PROJECT_PREFIX}" )

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


# --------------------------------------------------------------------
# Generate our ReadMe and License Files
configure_file(${PROJECT_SOURCE_DIR}/License.txt.in
                ${PROJECT_BINARY_DIR}/License.txt )
configure_file(${PROJECT_SOURCE_DIR}/ReadMe.txt.in
                ${PROJECT_BINARY_DIR}/ReadMe.txt )
IDE_SOURCE_PROPERTIES( "EMMPM/Configured" "${TO79_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Version.h;${TO79_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Configuration.h;${TO79_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Types.h" "")                

   
# Add our Qt based GUI project to the list of targets
ADD_SUBDIRECTORY( ${TO79_SOURCE_DIR}/Source/EMMPM/GUI ${TO79_BINARY_DIR}/GUI)

include(${PROJECT_CMAKE_DIR}/InstallationSupport/InstallSupportLibraries.cmake)
include(${PROJECT_RESOURCES_DIR}/CMake/PackageProject.cmake)

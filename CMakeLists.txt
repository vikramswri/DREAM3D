#///////////////////////////////////////////////////////////////////////////////
#
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#
#///////////////////////////////////////////////////////////////////////////////

# ------------------------------------------------------------------------------
# This project has dependencies on ITK. If you build ITK as Dynamic Libraries then
# the app bundling cmake code (at least for OS X) will most likely NOT be able to
# find those libraries. Building ITK as STATIC libraries will alleviate this 
# problem and allow you to create a self contained OS X app bundle with out any
# additional issues.
# ------------------------------------------------------------------------------

project (IPHelper)
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

SET (IPHelper_PROJECT_DIR ${IPHelper_SOURCE_DIR})
SET (IPHelper_CODE_DIR ${IPHelper_SOURCE_DIR}/Code)
SET (PROJECT_RESOURCES_DIR ${IPHelper_SOURCE_DIR}/Resources)
SET (PROJECT_CMAKE_DIR  ${PROJECT_RESOURCES_DIR}/MXACMake)
SET (OSX_TOOLS_DIR ${PROJECT_RESOURCES_DIR}/OSX_Tools)
SET (PROJECT_PREFIX "IPHelper" CACHE STRING "The 3 Letter Prefix to be used for Preprocessor definitions")
SET (EXE_DEBUG_EXTENSION "_debug")
SET (MXA_INSTALL_FILES "0")
SET (MXA_SOURCE_DIR ${IPHelper_CODE_DIR})

file(WRITE ${IPHelper_BINARY_DIR}/plugins.txt "")
file(WRITE ${IPHelper_BINARY_DIR}/libsearchdirs.txt "${IPHelper_BINARY_DIR}/Bin/plugins;")
file(APPEND ${IPHelper_BINARY_DIR}/libsearchdirs.txt "${IPHelper_BINARY_DIR}/Bin;")
# --------------------------------------------------------------------
# including this file will kick off all the type, header and other configuration
# checks that need to be performed in order to include all the MXA Files
INCLUDE (${PROJECT_CMAKE_DIR}/MXACMakeCommon.cmake )


# Include some directories for the compiler
include_directories(${PROJECT_BINARY_DIR})

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
GenerateVersionString( ${PROJECT_NAME} "${PROJECT_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Version.h" "${PROJECT_PREFIX}" )

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
# Find the ITK Package
# --------------------------------------------------------------------
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${USE_ITK_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

# --------------------------------------------------------------------
# Generate our ReadMe and License Files
configure_file(${PROJECT_SOURCE_DIR}/License.txt.in
                ${PROJECT_BINARY_DIR}/License.txt )
configure_file(${PROJECT_SOURCE_DIR}/ReadMe.txt.in
                ${PROJECT_BINARY_DIR}/ReadMe.txt )
IDE_SOURCE_PROPERTIES( "EMMPM/Configured" "${PROJECT_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Version.h;${PROJECT_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Configuration.h;${PROJECT_BINARY_DIR}/${PROJECT_PREFIX}/Common/${PROJECT_PREFIX}Types.h" "")                

# Compile the support libraries
add_subdirectory(${IPHelper_CODE_DIR}/AIM ${PROJECT_BINARY_DIR}/AIMLib)
add_subdirectory(${IPHelper_CODE_DIR}/QtSupport ${PROJECT_BINARY_DIR}/QtSupport)
add_subdirectory(${IPHelper_CODE_DIR}/MXA ${PROJECT_BINARY_DIR}/MXA)

# Compile the image processing plugins
add_subdirectory(${IPHelper_CODE_DIR}/IPHelper/plugins/EMMPM ${PROJECT_BINARY_DIR}/plugins/EMMPM)
add_subdirectory(${IPHelper_CODE_DIR}/IPHelper/plugins/CrossCorrelation ${PROJECT_BINARY_DIR}/plugins/CrossCorrelation)
add_subdirectory(${IPHelper_CODE_DIR}/IPHelper/plugins/MutualInformation ${PROJECT_BINARY_DIR}/plugins/MutualInformation)

# Compile the main Application - this MUST be last due to how some variables are built up.
add_subdirectory(${IPHelper_CODE_DIR}/IPHelper/Application ${PROJECT_BINARY_DIR}/Application)

# Should we build the HTML documentation
option(IPHelper_BUILD_API_DOCS "Use Doxygen to create the HTML based API documentation" OFF)
if(IPHelper_BUILD_API_DOCS)
  FIND_PACKAGE(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
  endif()
  configure_file(${PROJECT_RESOURCES_DIR}/IPHelper.doxyfile.in 
                 ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
  add_custom_target(docs COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)
endif()

include(${PROJECT_CMAKE_DIR}/InstallationSupport/InstallSupportLibraries.cmake)
include(${PROJECT_RESOURCES_DIR}/CPack/PackageProject.cmake)

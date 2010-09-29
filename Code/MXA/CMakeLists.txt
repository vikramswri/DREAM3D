#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(MXADataModel)

set (MXA_SOURCE_DIR ${PROJECT_CODE_DIR} )
set (Project_SRCS "")
set (Project_HDRS "")

Macro(MXACompileDir DirName)
    include(${MXADataModel_SOURCE_DIR}/${DirName}/SourceList.cmake)
    set (Project_SRCS ${Project_SRCS} ${MXA_${DirName}_SRCS} ${MXA_${DirName}_HDRS})
endMacro()

MXACompileDir(Common)
MXACompileDir(Utilities)
MXACompileDir(BMPIO)

set (Project_SRCS 
    ${Project_SRCS} 
    )

include_directories(${MXADataModel_SOURCE_DIR}/..)
include_directories(${MXADataModel_BINARY_DIR})
include_directories(${MXADataModel_BINARY_DIR}/..)

set (MXA_HDF5_SUPPORT "0")
set (MXA_TIFF_SUPPORT "0")
set (MXA_XML_SUPPORT "0")
set (HDF5_VTK_BRIDGE "0")

configure_file(${PROJECT_RESOURCES_DIR}/MXACMake/MXAConfiguration.h.in
               ${CMP_HEADER_DIR}/${CMP_EXTRA_CONFIGURATION_FILE})
               
#add_definitions(-DQT_CORE_LIB)

# - Check to see if any other project is redefining the library names
IF ( NOT DEFINED  MXADATAMODEL_LIB_NAME)
  SET (MXADATAMODEL_LIB_NAME MXADataModel)
ENDIF ( NOT DEFINED MXADATAMODEL_LIB_NAME )

add_library(${MXADATAMODEL_LIB_NAME} ${LIB_TYPE}  ${Project_SRCS})
target_link_libraries(${MXADATAMODEL_LIB_NAME} ${QT_LIBRARIES})

LibraryProperties( ${MXADATAMODEL_LIB_NAME} ${EXE_DEBUG_EXTENSION})

# --------------------------------------------------------------------
# Option to enable the BMP to TIFF utility
option(MXA_COMPILE_BMP2GRAYSCALETIFF "Compile a utility Application that will convert the BMP to a GrayScale TIFF with 72 DPI resolution" OFF)
if (MXA_COMPILE_BMP2GRAYSCALETIFF)
    # Prepend our own "FindTiff" code in front of what CMake itself provides
    set(CMAKE_MODULE_PATH ${PROJECT_CMAKE_DIR}/Modules ${CMAKE_MODULE_PATH} )
    FIND_PACKAGE(Tiff)
    IF (NOT TIFF_FOUND)
          MESSAGE (FATAL_ERROR " Tiff Library was NOT Found and is needed.")
    ENDIF (NOT TIFF_FOUND)
    INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIRS})
    add_executable(bmp2tiff Utilities/BmpToTiff.cpp)
    target_link_libraries(bmp2tiff ${MXADATAMODEL_LIB_NAME} ${TIFF_LIBRARY})
endif()


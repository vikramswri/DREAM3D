project(MXADataModel)


set (Project_SRCS "")
set (Project_HDRS "")

Macro(MXACompileDir DirName)
    include(${MXADataModel_SOURCE_DIR}/${DirName}/SourceList.cmake)
    set (Project_SRCS ${Project_SRCS} ${MXA_${DirName}_SRCS})
endMacro()

MXACompileDir(Common)
MXACompileDir(BMPIO)
MXACompileDir(Utilities)
MXACompileDir(BMPIO)

INCLUDE (${PROJECT_CMAKE_DIR}/MXACMakeCommon.cmake )


# add some useful macros and variables
# (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that 
# contains a path to CMake script)
INCLUDE( ${QT_USE_FILE} )

INCLUDE(${MXADataModel_SOURCE_DIR}/Qt/SourceList.cmake)
INCLUDE(${TO79_CODE_DIR}/AIM/Common/SourceList.cmake)

set (Project_SRCS ${Project_SRCS} 
        ${MXA_QT_SRCS} 
        ${MXA_QT_HDRS}
        ${QMXA_Generated_UI_HDRS}
        ${QMXA_Generated_MOC_SRCS}
        ${AIM_COMMON_SRCS}
        ${AIM_COMMON_HDRS}
        )

include_directories(${MXADataModel_SOURCE_DIR}/..)
include_directories(${MXADataModel_BINARY_DIR})
include_directories(${MXADataModel_BINARY_DIR}/..)
add_definitions(-DQT_CORE_LIB)

# - Check to see if any other project is redefining the library names
IF ( NOT DEFINED  MXADATAMODEL_LIB_NAME)
  SET (MXADATAMODEL_LIB_NAME MXADataModel)
ENDIF ( NOT DEFINED MXADATAMODEL_LIB_NAME )

add_library(${MXADATAMODEL_LIB_NAME} SHARED ${Project_SRCS})
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


#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(AIMLib)

set (Project_SRCS "")

Macro(MXACompileDir DirName)
    include(${AIMLib_SOURCE_DIR}/${DirName}/SourceList.cmake)
    set (Project_SRCS ${Project_SRCS} ${AIM_${DirName}_SRCS} ${AIM_${DirName}_HDRS})
endmacro()

MXACompileDir(Common)
MXACompileDir(ImageFilters)

include_directories(${AIMLib_SOURCE_DIR}/..)
include_directories(${AIMLib_BINARY_DIR})
include_directories(${AIMLib_BINARY_DIR}/..)
add_definitions(-DQT_CORE_LIB)


# --------------------------------------------------------------------
# Find the ITK Package
# --------------------------------------------------------------------
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${USE_ITK_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)



add_library(AIMLib STATIC ${Project_SRCS})
target_link_libraries(AIMLib ${ITK_LIBRARIES} ${QT_QTCORE_LIBRARY})

LibraryProperties( AIMLib ${EXE_DEBUG_EXTENSION})

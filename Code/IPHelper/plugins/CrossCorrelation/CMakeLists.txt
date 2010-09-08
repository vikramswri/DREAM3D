#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(CrossCorrelationPlugin)

# --------------------------------------------------------------------
# Find the ITK Package
# --------------------------------------------------------------------
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${USE_ITK_FILE})
  # Add the ITK library directories to the directory search paths for fixing
  # up OS X bundles.
  file(APPEND ${IPHelper_BINARY_DIR}/libsearchdirs.txt "${ITK_LIBRARY_DIRS};")
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/..)
include_directories(${IPHelper_CODE_DIR} )
include_directories(${IPHelper_CODE_DIR}/QtSupport )

include_directories(${IPHelper_BINARY_DIR})
include_directories(${IPHelper_BINARY_DIR}/QtSupport)

set (CrossCorrelationPlugin_HDRS
    ${PROJECT_SOURCE_DIR}/CrossCorrelationPlugin.h
    ${PROJECT_SOURCE_DIR}/CrossCorrelationInputUI.h
    ${PROJECT_SOURCE_DIR}/CrossCorrelationTask.h
    ${PROJECT_SOURCE_DIR}/../QImageProcessingInputFrame.h
    )

set (CrossCorrelationPlugin_SRCS 
    ${PROJECT_SOURCE_DIR}/CrossCorrelationPlugin.cpp
    ${PROJECT_SOURCE_DIR}/CrossCorrelationInputUI.cpp
    ${PROJECT_SOURCE_DIR}/CrossCorrelationTask.cpp
    ${PROJECT_SOURCE_DIR}/../QImageProcessingInputFrame.cpp
    )
    
QT4_WRAP_UI( CrossCorrelationPlugin_Generated_UI_HDRS 
    ${PROJECT_SOURCE_DIR}/CrossCorrelationInputUI.ui
    )

QT4_WRAP_CPP( CrossCorrelationPlugin_Generated_MOC_SRCS ${CrossCorrelationPlugin_HDRS} )

IDE_SOURCE_PROPERTIES("Code/" "${CrossCorrelationPlugin_HDRS}" "${CrossCorrelationPlugin_SRCS}")
IDE_SOURCE_PROPERTIES("Generated" "${CrossCorrelationPlugin_Generated_UI_HDRS}" "${CrossCorrelationPlugin_Generated_MOC_SRCS}")

set (CrossCorrelationPlugin_EXTRA_HDRS
    ${CrossCorrelationPlugin_SOURCE_DIR}/CrossCorrelation.h
    ${CrossCorrelationPlugin_SOURCE_DIR}/CrossCorrelationData.h
    ${CrossCorrelationPlugin_SOURCE_DIR}/CrossCorrelationTable.h
	)
set (CrossCorrelationPlugin_EXTRA_SRCS
    ${CrossCorrelationPlugin_SOURCE_DIR}/CrossCorrelation.cpp
    ${CrossCorrelationPlugin_SOURCE_DIR}/CrossCorrelationData.cpp
    ${CrossCorrelationPlugin_SOURCE_DIR}/CrossCorrelationTable.cpp
	)

IDE_SOURCE_PROPERTIES("Code/" "${CrossCorrelationPlugin_EXTRA_HDRS}" "${CrossCorrelationPlugin_EXTRA_SRCS}")

set (CrossCorrelationPlugin_PROJECT_SRCS
    ${CrossCorrelationPlugin_EXTRA_SRCS}
	${CrossCorrelationPlugin_EXTRA_HDRS}
    ${CrossCorrelationPlugin_SRCS}
    ${CrossCorrelationPlugin_HDRS}
    ${CrossCorrelationPlugin_Generated_MOC_SRCS} 
    ${CrossCorrelationPlugin_Generated_UI_HDRS}
)

# ---------- Setup output Directories -------------------------
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${IPHelper_BINARY_DIR}/Bin/Plugins  )
add_library(CrossCorrelationPlugin MODULE ${CrossCorrelationPlugin_PROJECT_SRCS})
target_link_libraries(CrossCorrelationPlugin 
                    ${QT_LIBRARIES} 
                    ${ITK_LIBRARIES} 
                    QtSupport
                    MXADataModel 
                    AIMLib)
PluginProperties(CrossCorrelationPlugin "_debug")

SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${IPHelper_BINARY_DIR}/Bin  )


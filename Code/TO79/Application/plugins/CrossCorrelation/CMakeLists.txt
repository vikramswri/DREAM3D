project(CrossCorrelationPlugin)

# --------------------------------------------------------------------
# Find the ITK Package
# --------------------------------------------------------------------
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${USE_ITK_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

include_directories( ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/..)


set (CrossCorrelationPlugin_HDRS
    ${TO79MainWindow_SOURCE_DIR}/plugins/CrossCorrelation/CrossCorrelationPlugin.h
    ${TO79MainWindow_SOURCE_DIR}/plugins/CrossCorrelation/CrossCorrelationInputUI.h
    ${TO79MainWindow_SOURCE_DIR}/plugins/CrossCorrelation/CrossCorrelationTask.h
    )

set (CrossCorrelationPlugin_SRCS 
    ${TO79MainWindow_SOURCE_DIR}/plugins/CrossCorrelation/CrossCorrelationPlugin.cpp
    ${TO79MainWindow_SOURCE_DIR}/plugins/CrossCorrelation/CrossCorrelationInputUI.cpp
    ${TO79MainWindow_SOURCE_DIR}/plugins/CrossCorrelation/CrossCorrelationTask.cpp
    )
    
QT4_WRAP_UI( CrossCorrelationPlugin_Generated_UI_HDRS 
        ${TO79MainWindow_SOURCE_DIR}/plugins/CrossCorrelation/CrossCorrelationInputUI.ui )

QT4_WRAP_CPP( CrossCorrelationPlugin_Generated_MOC_SRCS ${CrossCorrelationPlugin_HDRS} )

set (CrossCorrelationPlugin_PROJECT_SRCS
    ${CrossCorrelationPlugin_SOURCE_DIR}/CrossCorrelation.cpp
    ${CrossCorrelationPlugin_SOURCE_DIR}/CrossCorrelationData.cpp
    ${CrossCorrelationPlugin_SOURCE_DIR}/CrossCorrelationTable.cpp
    ${CrossCorrelationPlugin_SOURCE_DIR}/ImageFilters/R3DImageFilter.cpp
    ${CrossCorrelationPlugin_SOURCE_DIR}/ImageFilters/R3DCropGrayScaleImage.cpp
    ${CrossCorrelationPlugin_SOURCE_DIR}/ImageFilters/R3DImagePadFilter.cpp
    ${CrossCorrelationPlugin_SRCS}
    ${CrossCorrelationPlugin_HDRS}
    ${CrossCorrelationPlugin_Generated_MOC_SRCS} 
    ${CrossCorrelationPlugin_Generated_UI_HDRS}
)

# ---------- Setup output Directories -------------------------
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${TO79_BINARY_DIR}/Bin/plugins  )
add_library(CrossCorrelationPlugin MODULE ${CrossCorrelationPlugin_PROJECT_SRCS})
target_link_libraries(CrossCorrelationPlugin ${QT_LIBRARIES} ${ITK_LIBRARIES} MXADataModel)
PluginProperties(CrossCorrelationPlugin "_debug")
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${TO79_BINARY_DIR}/Bin  )


project(EMMPMPlugin)

include_directories( ${PROJECT_SOURCE_DIR}  ${PROJECT_BINARY_DIR})

set (EMMPMPlugin_HDRS
    ${TO79MainWindow_SOURCE_DIR}/plugins/EMMPM/EMMPMPlugin.h
    ${TO79MainWindow_SOURCE_DIR}/plugins/EMMPM/EMMPMInputUI.h
    ${TO79MainWindow_SOURCE_DIR}/plugins/EMMPM/EMMPMTask.h
    )
    
set (EMMPMPlugin_SRCS 
    ${TO79MainWindow_SOURCE_DIR}/plugins/EMMPM/EMMPMPlugin.cpp
    ${TO79MainWindow_SOURCE_DIR}/plugins/EMMPM/EMMPMInputUI.cpp
    ${TO79MainWindow_SOURCE_DIR}/plugins/EMMPM/EMMPMTask.cpp
    )
QT4_WRAP_UI( EMMPMPlugin_Generated_UI_HDRS 
            ${TO79MainWindow_SOURCE_DIR}/plugins/EMMPM/EMMPMInputUI.ui )

QT4_WRAP_CPP( EMMPMPlugin_Generated_MOC_SRCS ${EMMPMPlugin_HDRS} )

set (EMMPMPlugin_PROJECT_SRCS
    ${EMMPMPlugin_SRCS}
    ${EMMPMPlugin_HDRS}
    ${EMMPMPlugin_Generated_MOC_SRCS} 
    ${EMMPMPlugin_Generated_UI_HDRS}
)

# ---------- Setup output Directories -------------------------
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${TO79_BINARY_DIR}/Bin/plugins  )
add_library(EMMPMPlugin MODULE ${EMMPMPlugin_PROJECT_SRCS})
target_link_libraries(EMMPMPlugin ${QT_LIBRARIES} MXADataModel)
PluginProperties(EMMPMPlugin "_debug")
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${TO79_BINARY_DIR}/Bin  )


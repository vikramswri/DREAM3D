project(EMMPMPlugin)

include_directories(${PROJECT_SOURCE_DIR}  ${PROJECT_BINARY_DIR})
include_directories( ${TO79_CODE_DIR} )
include_directories(${TO79_BINARY_DIR})
include_directories(${TO79_BINARY_DIR}/MXA)


set (EMMPMPlugin_HDRS
    ${PROJECT_SOURCE_DIR}/EMMPMPlugin.h
    ${PROJECT_SOURCE_DIR}/EMMPMInputUI.h
    ${PROJECT_SOURCE_DIR}/EMMPMTask.h
    ${PROJECT_SOURCE_DIR}/../QImageProcessingInputFrame.h
    )
    
set (EMMPMPlugin_SRCS 
    ${PROJECT_SOURCE_DIR}/EMMPMPlugin.cpp
    ${PROJECT_SOURCE_DIR}/EMMPMInputUI.cpp
    ${PROJECT_SOURCE_DIR}/EMMPMTask.cpp
    ${PROJECT_SOURCE_DIR}/../QImageProcessingInputFrame.cpp
    )
QT4_WRAP_UI( EMMPMPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/EMMPMInputUI.ui )

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


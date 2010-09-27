#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(MutualInformation)

include_directories(${PROJECT_SOURCE_DIR}  ${PROJECT_BINARY_DIR})
include_directories(${IPHelper_CODE_DIR} )
include_directories(${IPHelper_BINARY_DIR})
include_directories(${IPHelper_BINARY_DIR}/MXA)


set (MutualInformation_HDRS
    ${PROJECT_SOURCE_DIR}/MutualInformation.h
    ${PROJECT_SOURCE_DIR}/EMMPMInputUI.h
    ${PROJECT_SOURCE_DIR}/EMMPMTask.h
    ${PROJECT_SOURCE_DIR}/../QImageProcessingInputFrame.h
    )
    
set (MutualInformation_SRCS 
    ${PROJECT_SOURCE_DIR}/MutualInformation.cpp
    ${PROJECT_SOURCE_DIR}/EMMPMInputUI.cpp
    ${PROJECT_SOURCE_DIR}/EMMPMTask.cpp
    ${PROJECT_SOURCE_DIR}/../QImageProcessingInputFrame.cpp
    )
QT4_WRAP_UI( MutualInformation_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/EMMPMInputUI.ui )

QT4_WRAP_CPP( MutualInformation_Generated_MOC_SRCS ${MutualInformation_HDRS} )

set (MutualInformation_PROJECT_SRCS
#    ${MutualInformation_SRCS}
#    ${MutualInformation_HDRS}
#    ${MutualInformation_Generated_MOC_SRCS} 
#    ${MutualInformation_Generated_UI_HDRS}
    ${PROJECT_SOURCE_DIR}/MITest.cpp
    ${PROJECT_SOURCE_DIR}/CMUMutualInformation.cpp
    ${PROJECT_SOURCE_DIR}/CMUMutualInformation.h
)

# ---------- Setup output Directories -------------------------
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${IPHelper_BINARY_DIR}/Bin/Plugins  )
#add_library(MutualInformation STATIC ${MutualInformation_PROJECT_SRCS})
add_executable(MITest ${MutualInformation_PROJECT_SRCS})
target_link_libraries(MITest ${QT_LIBRARIES} MXADataModel AIMLib)
#PluginProperties(MutualInformation "_debug")
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${IPHelper_BINARY_DIR}/Bin  )


# --------------------------------------------------------------------
#  Add an executable based on the ITK Mutual Information Classes
set (ITK_MI_TEST_SRCS  
    ${IPHelper_CODE_DIR}/AngReader/AngDirectoryPatterns.cpp
    ${IPHelper_CODE_DIR}/AngReader/AngFileHelper.cpp
    ${IPHelper_CODE_DIR}/AngReader/AngReader.cpp
    ${IPHelper_CODE_DIR}/AngReader/itkAngImageIO.cpp
    ${IPHelper_CODE_DIR}/AngReader/itkAngImageIOFactory.cpp
    ${PROJECT_SOURCE_DIR}/itkMITest.cpp
)

set (ITK_MI_TEST_HDRS  
    ${IPHelper_CODE_DIR}/AngReader/AngConstants.h
    ${IPHelper_CODE_DIR}/AngReader/AngDirectoryPatterns.h
    ${IPHelper_CODE_DIR}/AngReader/AngFileHelper.h
    ${IPHelper_CODE_DIR}/AngReader/AngReader.h
    ${IPHelper_CODE_DIR}/AngReader/AngUtils.hpp
    ${IPHelper_CODE_DIR}/AngReader/OIMColoring.hpp
    ${IPHelper_CODE_DIR}/AngReader/itkAngImageIO.h
    ${IPHelper_CODE_DIR}/AngReader/itkAngImageIOFactory.h
)

add_executable(itkMITest ${ITK_MI_TEST_SRCS} ${ITK_MI_TEST_HDRS} )
target_link_libraries(itkMITest ${ITK_LIBRARIES} ${QT_QTCORE_LIBRARY})

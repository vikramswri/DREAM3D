 # <distribution-statement>
 # #############################################################################
 # Distribution authorized to DOD components and DOD contractors only;         #
 # Administrative or Operational Use;                                          #
 # February 2009.                                                              #
 # Other requests for this document shall be referred to                       #
 # U.S. Air Force Research Laboratory                                          #
 # 2230 Tenth St., Ste. 1                                                      #
 # Wright-Patterson AFB, OH 45433-7817                                         #
 #                                                                             #
 # Destroy by contents or reconstruction of the document.                      #
 #                                                                             #
 # This code was written under the U.S. Air Force contrct F33615-03-C-5220     #
 # #############################################################################
 # </distribution-statement> 
 # set project's name
PROJECT( CrossCorrelation )
cmake_minimum_required(VERSION 2.6)

if (NOT DEFINED OUTPUT_DIRS_SET)
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
endif()

IF (NOT DEFINED CrossCorrelation_SOURCE_DIR)
    SET (CrossCorrelation_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
ENDIF(NOT DEFINED CrossCorrelation_SOURCE_DIR)


set (PCM_CODE_DIR ${CrossCorrelation_SOURCE_DIR}/pcm)

INCLUDE_DIRECTORIES(${CrossCorrelation_SOURCE_DIR}/pcm)

# List the source files here
SET (CrossCorrelation_SRCS
    ${CrossCorrelation_SOURCE_DIR}/CrossCorrelationData.cpp
    ${CrossCorrelation_SOURCE_DIR}/CrossCorrelationTable.cpp
    ${CrossCorrelation_SOURCE_DIR}/CrossCorrelation.cpp
    ${CrossCorrelation_SOURCE_DIR}/CrossCorrelationThread.cpp
)

SET (CrossCorrelation_HDRS
    ${CrossCorrelation_SOURCE_DIR}/CrossCorrelationData.h
    ${CrossCorrelation_SOURCE_DIR}/CrossCorrelationTable.h
    ${CrossCorrelation_SOURCE_DIR}/CrossCorrelation.h
    ${CrossCorrelation_SOURCE_DIR}/CrossCorrelationThread.h
    ${PCM_CODE_DIR}/itkMaxPhaseCorrelationOptimizer.h
    ${PCM_CODE_DIR}/itkMaxPhaseCorrelationOptimizer.txx
    ${PCM_CODE_DIR}/itkPhaseCorrelationImageRegistrationMethod.h
    ${PCM_CODE_DIR}/itkPhaseCorrelationImageRegistrationMethod.txx
    ${PCM_CODE_DIR}/itkPhaseCorrelationOperator.h
    ${PCM_CODE_DIR}/itkPhaseCorrelationOperator.txx
    ${PCM_CODE_DIR}/itkPhaseCorrelationOptimizer.h
    ${PCM_CODE_DIR}/itkPhaseCorrelationOptimizer.txx  
)
R3D_SOURCE_PROPERTIES(CrossCorrelation "${CrossCorrelation_HDRS};" "${CrossCorrelation_SRCS}")

#-- Add a static library for the PCM registration
add_library(R3DCrossCorrelationLib STATIC ${CrossCorrelation_SRCS} ${CrossCorrelation_HDRS})
target_link_libraries(R3DCrossCorrelationLib ${ITK_LIBRARIES}
                                         R3DCommon
                                         R3DThreads )
StaticLibraryProperties( R3DCrossCorrelationLib )


ADD_EXECUTABLE(${PROJECT_NAME}  ${CrossCorrelation_SOURCE_DIR}/main.cpp )
target_link_libraries(${PROJECT_NAME}
                      R3DCrossCorrelationLib
                      R3DCommon
                      ${CMAKE_THREAD_LIBS_INIT}
                      ${Boost_THREAD_LIBRARY}
                      R3DThreads
                      ${ITK_LIBRARIES}
                      ${MXADATAMODEL_LIBRARIES}
                      ${EXPAT_LIBRARIES}
                      ${HDF5_LIBRARIES}
                      ${TIFF_LIBRARIES}  
                      ${Boost_PROGRAM_OPTIONS_LIBRARY})
R3D_SOURCE_PROPERTIES(CrossCorrelation "${CrossCorrelation_HDRS};" "${CrossCorrelation_SRCS};${CrossCorrelation_SOURCE_DIR}/main.cpp")                      
ToolInstallationSupport(${PROJECT_NAME} ${EXE_DEBUG_EXTENSION} ${CrossCorrelation_BINARY_DIR})

#-- Add our GUI Wrapper program
if(R3D_BUILD_QT_APPS)
  #  add_subdirectory(${CrossCorrelation_SOURCE_DIR}/GUI ${CrossCorrelation_BINARY_DIR}/R3DPcmRegistration )
endif(R3D_BUILD_QT_APPS)


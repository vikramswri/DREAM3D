#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(AngReader)

IF(ITK_FOUND)
  INCLUDE(${USE_ITK_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)



set (Project_SRCS "")

set (AngReader_SRCS
    ${AngReader_SOURCE_DIR}/AngDirectoryPatterns.cpp
    ${AngReader_SOURCE_DIR}/AngFileHelper.cpp
    ${AngReader_SOURCE_DIR}/AngReader.cpp
#    ${AngReader_SOURCE_DIR}/itkAngImageIO.cpp
#    ${AngReader_SOURCE_DIR}/itkAngImageIOFactory.cpp
)

set (AngReader_HDRS
    ${AngReader_SOURCE_DIR}/AngConstants.h
    ${AngReader_SOURCE_DIR}/AngDirectoryPatterns.h
    ${AngReader_SOURCE_DIR}/AngFileHelper.h
    ${AngReader_SOURCE_DIR}/AngReader.h
#    ${AngReader_SOURCE_DIR}/itkAngImageIO.h
#    ${AngReader_SOURCE_DIR}/itkAngImageIOFactory.h
    ${AngReader_SOURCE_DIR}/OIMColoring.hpp
)

cmp_IDE_SOURCE_PROPERTIES("AngReader" "${AngReader_HDRS}" "${AngReader_SRCS}" "0")


include_directories(${AngReader_SOURCE_DIR}/..)
include_directories(${AngReader_BINARY_DIR})
include_directories(${AngReader_BINARY_DIR}/..)


set (Project_SRCS ${Project_SRCS} ${AngReader_SRCS} ${AngReader_HDRS})

add_library(AngReader STATIC ${Project_SRCS})
target_link_libraries(AngReader MXADataModel)

LibraryProperties( AngReader ${EXE_DEBUG_EXTENSION})




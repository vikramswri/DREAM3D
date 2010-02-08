#///////////////////////////////////////////////////////////////////////////////
#
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#
#///////////////////////////////////////////////////////////////////////////////

#-------------------------------------------------------------------------------
MACRO (IDE_GENERATED_PROPERTIES SOURCE_PATH HEADERS SOURCES)
    STRING(REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH}  )
    source_group(${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #set_property(SOURCE ${HEADERS}
  #             PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)

ENDMACRO (IDE_GENERATED_PROPERTIES SOURCE_PATH HEADERS SOURCES)


#-------------------------------------------------------------------------------

MACRO (IDE_SOURCE_PROPERTIES SOURCE_PATH HEADERS SOURCES)

    STRING(REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH}  )
    source_group(${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #set_property(SOURCE ${HEADERS}
  #             PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)

ENDMACRO (IDE_SOURCE_PROPERTIES NAME HEADERS SOURCES)

# --------------------------------------------------------------------
#///////////////////////////////////////////////////////////////////////////////
#//
#//  Copyright (c) 2008, Michael A. Jackson. BlueQuartz Software
#//  All rights reserved.
#//  BSD License: http://www.opensource.org/licenses/bsd-license.html
#//
#///////////////////////////////////////////////////////////////////////////////
# ------------------------------------------------------------------------------ 
# This CMake code installs the needed support libraries
# ------------------------------------------------------------------------------ 
macro(InstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR)

    SET_TARGET_PROPERTIES( ${EXE_NAME} 
        PROPERTIES
        DEBUG_OUTPUT_NAME ${EXE_NAME}${EXE_DEBUG_EXTENSION}
        RELEASE_OUTPUT_NAME ${EXE_NAME}
    )
    
    
    INSTALL(TARGETS ${EXE_NAME} 
        RUNTIME
        DESTINATION ./
        COMPONENT Applications
        LIBRARY DESTINATION ./ 
        ARCHIVE DESTINATION ./
        RUNTIME DESTINATION ./
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/.
    )   
    
    # --- If we are on OS X copy all the embedded libraries to the app bundle
    if(DEFINED GUI_TYPE)
       # message(STATUS "Creating Install CMake file for GUI application ${EXE_NAME}")
        if(${GUI_TYPE} STREQUAL "MACOSX_BUNDLE")
            include (${OSX_TOOLS_DIR}/OSX_BundleTools.cmake)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXBundleApp( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                    ${EXE_BINARY_DIR}
                                    ${OSX_TOOLS_DIR} )
            else (CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXBundleApp(${EXE_NAME} 
                                 ${EXE_BINARY_DIR}
                                 ${OSX_TOOLS_DIR} )
            endif()
        endif()
    else(DEFINED GUI_TYPE)
      #  message(STATUS "Creating Install CMake file for tool application ${EXE_NAME}")
        if (APPLE)
            include (${OSX_TOOLS_DIR}/OSX_BundleTools.cmake)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                    ${EXE_BINARY_DIR}
                                    ${OSX_TOOLS_DIR} )
            else (CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool(${EXE_NAME} 
                                 ${EXE_BINARY_DIR}
                                 ${OSX_TOOLS_DIR} )
            endif()
        endif(APPLE)
    endif(DEFINED GUI_TYPE)
endmacro(InstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR)

# --------------------------------------------------------------------
#
# --------------------------------------------------------------------
macro(ToolInstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR)

    SET_TARGET_PROPERTIES( ${EXE_NAME} 
        PROPERTIES
        DEBUG_OUTPUT_NAME ${EXE_NAME}${EXE_DEBUG_EXTENSION}
        RELEASE_OUTPUT_NAME ${EXE_NAME}
    )
    
    if (APPLE)
        set(TOOL_INSTALL_LOCATION "./")
    else()
        set(TOOL_INSTALL_LOCATION "./")
    endif()
    
    INSTALL(TARGETS ${EXE_NAME} 
        RUNTIME
        DESTINATION ${TOOL_INSTALL_LOCATION}
        COMPONENT Tools
        LIBRARY DESTINATION ${TOOL_INSTALL_LOCATION} 
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION ${TOOL_INSTALL_LOCATION}
        BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/.
    )   
    
    # --- If we are on OS X copy all the embedded libraries to the app bundle
    if(DEFINED GUI_TYPE)
      #  message(STATUS "Creating Install CMake file for GUI application ${EXE_NAME}")
        if(${GUI_TYPE} STREQUAL "MACOSX_BUNDLE")
            include (${OSX_TOOLS_DIR}/OSX_BundleTools.cmake)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXBundleApp( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                    ${EXE_BINARY_DIR}
                                    ${OSX_TOOLS_DIR} )
            else (CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXBundleApp(${EXE_NAME} 
                                 ${EXE_BINARY_DIR}
                                 ${OSX_TOOLS_DIR} )
            endif()
        endif()
    else(DEFINED GUI_TYPE)
     #   message(STATUS "Creating Install CMake file for tool application ${EXE_NAME}")
        if (APPLE)
            include (${OSX_TOOLS_DIR}/OSX_BundleTools.cmake)
            if(CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool( "${EXE_NAME}${EXE_DEBUG_EXTENSION}" 
                                    ${EXE_BINARY_DIR}
                                    ${OSX_TOOLS_DIR} )
            else (CMAKE_BUILD_TYPE MATCHES "Debug")
                MakeOSXTool(${EXE_NAME} 
                                 ${EXE_BINARY_DIR}
                                 ${OSX_TOOLS_DIR} )
            endif()
        endif(APPLE)
    endif(DEFINED GUI_TYPE)
endmacro(ToolInstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR)


# --------------------------------------------------------------------
#
#
macro(StaticLibraryProperties targetName )
    if (WIN32 AND NOT MINGW)
        SET (DEBUG_EXTENSION "_d")
    else()
        SET (DEBUG_EXTENSION "_debug")
    endif()
    
    
    IF (WIN32 AND NOT MINGW)
        SET(LIBRARY_RELEASE_NAME "lib${targetName}" CACHE INTERNAL "" FORCE)
        SET(LIBRARY_DEBUG_NAME "lib${targetName}${DEBUG_EXTENSION}" CACHE INTERNAL "" FORCE)
    ELSE (WIN32 AND NOT MINGW)
        SET(LIBRARY_RELEASE_NAME "${targetName}" CACHE INTERNAL "" FORCE)
        SET(LIBRARY_DEBUG_NAME "${targetName}${DEBUG_EXTENSION}" CACHE INTERNAL "" FORCE)
    ENDIF(WIN32 AND NOT MINGW)
    
    
    #-- Set the Debug and Release names for the libraries
    SET_TARGET_PROPERTIES( ${targetName} 
        PROPERTIES
        DEBUG_OUTPUT_NAME ${LIBRARY_DEBUG_NAME}
        RELEASE_OUTPUT_NAME ${LIBRARY_RELEASE_NAME}
    )

endmacro(StaticLibraryProperties)


#-------------------------------------------------------------------------------
# This macro will attempt a try_run command in order to compile and then 
# generate a version string based on today's date. The output string should be
# of the form YYYY.MM.DD. 
#  Required CMake variables to be set are:
#   EmInit_CMAKE_DIR - The path to the MXA CMake directory
#  The following variables are set, all of which should have been already
#  initialized to a default value
#   ${PROJECT_NAME}_VERSION
#   ${PROJECT_NAME}_VER_MAJOR
#   ${PROJECT_NAME}_VER_MINOR
#   ${PROJECT_NAME}_VER_PATCH
#
#-------------------------------------------------------------------------------
macro(GenerateVersionString PROJECT_NAME GENERATED_FILE_PATH NAMESPACE )
#    message(STATUS "Generating Versions String for ${PROJECT_NAME}")
#    message(STATUS "EmInit_CMAKE_DIR: ${EmInit_CMAKE_DIR}")
    try_run(RUN_RESULT_VAR COMPILE_RESULT_VAR 
            ${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_RESOURCES_DIR}/CMake/GenerateVersionString.cpp
            COMPILE_DEFINITIONS ${VERSION_COMPILE_FLAGS}
            COMPILE_OUTPUT_VARIABLE VERSION_COMPILE_OUTPUT
            RUN_OUTPUT_VARIABLE VERSION_GEN_COMPLETE )
   
    # and now the version string given by qmake
    STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" VERSION_GEN_VER_MAJOR "${VERSION_GEN_COMPLETE}")
    STRING(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]+.*" "\\1" VERSION_GEN_VER_MINOR "${VERSION_GEN_COMPLETE}")
    STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_GEN_VER_PATCH "${VERSION_GEN_COMPLETE}")
    
    set (VERSION_GEN_NAME "${PROJECT_NAME}")
    set (VERSION_GEN_NAMESPACE "${NAMESPACE}")
    set (${PROJECT_NAME}_VERSION   ${VERSION_GEN_COMPLETE}  CACHE STRING "Complete Version String")
    set (${PROJECT_NAME}_VER_MAJOR ${VERSION_GEN_VER_MAJOR} CACHE STRING "Major Version String")
    set (${PROJECT_NAME}_VER_MINOR ${VERSION_GEN_VER_MINOR} CACHE STRING "Minor Version String")
    set (${PROJECT_NAME}_VER_PATCH ${VERSION_GEN_VER_PATCH} CACHE STRING "Patch Version String")
    configure_file(${PROJECT_RESOURCES_DIR}/CMake/Version.h.in
                   ${GENERATED_FILE_PATH})
               
endmacro()



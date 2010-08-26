#--////////////////////////////////////////////////////////////////////////////
#--  Copyright (c) 2009, 2010 Michael A. Jackson. BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--////////////////////////////////////////////////////////////////////////////

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
    if ( DEFINED MXA_INSTALL_FILES)
    if( ${MXA_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${HEADERS}
             DESTINATION include/${SOURCE_PATH}
             COMPONENT Headers           
    )
    endif()
    endif()
    STRING(REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH}  )
    source_group(${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #set_property(SOURCE ${HEADERS}
  #             PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)

ENDMACRO (IDE_SOURCE_PROPERTIES NAME HEADERS SOURCES)


# ------------------------------------------------------------------------------ 
# This CMake code installs the needed support libraries
# ------------------------------------------------------------------------------ 
macro(InstallationSupport EXE_NAME EXE_DEBUG_EXTENSION EXE_BINARY_DIR)

    SET_TARGET_PROPERTIES( ${EXE_NAME} 
        PROPERTIES
        DEBUG_OUTPUT_NAME ${EXE_NAME}${EXE_DEBUG_EXTENSION}
        RELEASE_OUTPUT_NAME ${EXE_NAME}
    )
    
    if ( DEFINED MXA_INSTALL_FILES)
        if ( ${MXA_INSTALL_FILES} EQUAL 1 )
            INSTALL(TARGETS ${EXE_NAME} 
                RUNTIME
                DESTINATION ./
                COMPONENT Applications
                LIBRARY DESTINATION ./ 
                ARCHIVE DESTINATION ./
                RUNTIME DESTINATION ./
                BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/.
            )   
        endif()
    endif()
    
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
        set(TOOL_INSTALL_LOCATION "bin")
    else()
        set(TOOL_INSTALL_LOCATION "bin")
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

macro(LibraryProperties targetName DEBUG_EXTENSION)
    if ( NOT BUILD_SHARED_LIBS AND MSVC)
      SET_TARGET_PROPERTIES( ${targetName} 
        PROPERTIES
        DEBUG_OUTPUT_NAME lib${targetName}
        RELEASE_OUTPUT_NAME lib${targetName}  )
    endif()
    
    #-- Set the Debug and Release names for the libraries
    SET_TARGET_PROPERTIES( ${targetName} 
        PROPERTIES
        DEBUG_POSTFIX ${DEBUG_EXTENSION} )
    
    IF (APPLE AND BUILD_SHARED_LIBS)
      OPTION (MXA_BUILD_WITH_INSTALL_NAME "Build Libraries with the install_name set to the installation prefix. This is good if you are going to run from the installation location" OFF)
      IF(MXA_BUILD_WITH_INSTALL_NAME)
      
          SET_TARGET_PROPERTIES(${MXADATAMODEL_LIB_NAME}
             PROPERTIES
             LINK_FLAGS "-current_version ${${PROJECT_NAME}_VERSION} -compatibility_version ${${PROJECT_NAME}_VERSION}"
             INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
             BUILD_WITH_INSTALL_RPATH ${MXA_BUILD_WITH_INSTALL_NAME}
          )
     ENDIF(MXA_BUILD_WITH_INSTALL_NAME)
   ENDIF (APPLE AND BUILD_SHARED_LIBS)

endmacro(LibraryProperties DEBUG_EXTENSION)

# --------------------------------------------------------------------
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


# --------------------------------------------------------------------
#-- Copy all the dependent DLLs into the current build directory so that the test
#-- can run.
MACRO (MXA_COPY_DEPENDENT_LIBRARIES mxa_lib_list)
  set (mxa_lib_list ${mxa_lib_list})
  SET (TYPES Debug Release)
  if (MSVC)
    # Make all the necessary intermediate directories for Visual Studio
    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel)
    file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo)
    FOREACH(lib ${mxa_lib_list})
      STRING(TOUPPER ${lib} upperlib)
      if (${upperlib}_IS_SHARED)
        FOREACH(BTYPE ${TYPES} )
        #  message(STATUS "Looking for ${BTYPE} DLL Version of ${lib_name}")
          STRING(TOUPPER ${BTYPE} TYPE)        
          get_filename_component(lib_path ${${upperlib}_LIBRARY_${TYPE}} PATH)
          get_filename_component(lib_name ${${upperlib}_LIBRARY_${TYPE}} NAME_WE)
        #  message(STATUS "lib_path: ${lib_path}")
        #  message(STATUS "lib_name: ${lib_name}")
          
          find_file(${upperlib}_LIBRARY_DLL_${TYPE}
                        NAMES ${lib_name}.dll
                        PATHS  ${lib_path}/../bin ${lib_path}/.. ${lib_path}/
                        NO_DEFAULT_PATH )
         # message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
          mark_as_advanced(${upperlib}_LIBRARY_DLL_${TYPE})
          if ( ${${upperlib}_LIBRARY_DLL_${TYPE}} STREQUAL  "${upperlib}_LIBRARY_DLL_${TYPE}-NOTFOUND")
            message(FATAL_ERROR "According to how ${upperlib}_LIBRARY_${TYPE} was found the library should"
                                " have been built as a DLL but no .dll file can be found. I looked in the "
                                " following locations:  ${lib_path}\n  ${lib_path}/..\n  ${lib_path}/../bin")
          endif()

         # SET(${upperlib}_LIBRARY_DLL_${TYPE} "${${upperlib}_LIBRARY_DLL_${TYPE}}/${lib_name}.dll" CACHE FILEPATH "The path to the DLL Portion of the library" FORCE)
         # message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
          message(STATUS "Generating Copy Rule for DLL File for ${upperlib}_LIBRARY_${TYPE}")
          ADD_CUSTOM_TARGET(ZZ_${upperlib}_DLL_${TYPE}-Copy ALL 
                      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${${upperlib}_LIBRARY_DLL_${TYPE}}
                      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BTYPE}/ 
                      COMMENT "  Copy: ${${upperlib}_LIBRARY_DLL_${TYPE}}\n    To: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${BTYPE}/")

        ENDFOREACH(BTYPE ${TYPES})
      ENDIF(${upperlib}_IS_SHARED)
    ENDFOREACH(lib ${mxa_lib_list})
  ENDIF(MSVC)  
    
endmacro()
#
# --------------------------------------------------------------------
# This macro generates install rules for Visual Studio builds so that
# dependent DLL libraries (HDF5, Tiff, Expat, MXADataModel) will be
# properly installed with your project.
# --------------------------------------------------------------------
MACRO (MXA_LIBRARIES_INSTALL_RULES mxa_lib_list destination)
  set (mxa_lib_list ${mxa_lib_list})
  SET (TYPES Debug Release)
  if (MSVC)
   
    FOREACH(lib ${mxa_lib_list})
        STRING(TOUPPER ${lib} upperlib)
      
        FOREACH(BTYPE ${TYPES} )
          STRING(TOUPPER ${BTYPE} TYPE)        
          get_filename_component(lib_path ${${upperlib}_LIBRARY_${TYPE}} PATH)
          get_filename_component(lib_name ${${upperlib}_LIBRARY_${TYPE}} NAME_WE)
          
          find_file(${upperlib}_LIBRARY_DLL_${TYPE}
                        NAMES ${lib_name}.dll
                        PATHS  ${lib_path}/../bin ${lib_path}/.. ${lib_path}/
                        NO_DEFAULT_PATH )
         # message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
          mark_as_advanced(${upperlib}_LIBRARY_DLL_${TYPE})
          if ( ${${upperlib}_LIBRARY_DLL_${TYPE}} STREQUAL  "${upperlib}_LIBRARY_DLL_${TYPE}-NOTFOUND")
            # message(STATUS "A Companion DLL for ${upperlib}_LIBRARY_${TYPE} was NOT found which usually means"
            #                    " that the library was NOT built as a DLL. I looked in the "
            #                    " following locations:  ${lib_path}\n  ${lib_path}/..\n  ${lib_path}/../bin")
          else()
             # set(${upperlib}_LIBRARY_DLL_${TYPE}  ${${upperlib}_LIBRARY_DLL_${TYPE}}/${lib_name}.dll)
             # message(STATUS "${upperlib}_LIBRARY_DLL_${TYPE}: ${${upperlib}_LIBRARY_DLL_${TYPE}}")
              message(STATUS "Generating Install Rule for DLL File for ${upperlib}_LIBRARY_${TYPE}")
              INSTALL(FILES ${${upperlib}_LIBRARY_DLL_${TYPE}}
                DESTINATION ${destination} 
                CONFIGURATIONS ${BTYPE} 
                COMPONENT Runtime)
          endif()
        
        ENDFOREACH(BTYPE ${TYPES})
    ENDFOREACH(lib ${mxa_lib_list})
  ENDIF(MSVC) 
ENDMACRO()

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
    # message(STATUS "Generating Version Strings for ${PROJECT_NAME}")
    SET(CMAKE_REQUIRED_INCLUDES_SAVE ${CMAKE_REQUIRED_INCLUDES})
    SET(CMAKE_REQUIRED_FLAGS_SAVE    ${CMAKE_REQUIRED_FLAGS})
    # Add MXADATAMODEL_INCLUDE_DIR to CMAKE_REQUIRED_INCLUDES
    SET(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES};${MXADATAMODEL_INCLUDE_DIRS}")

    CHECK_SYMBOL_EXISTS( MXA_HAVE_TIME_GETTIMEOFDAY "MXA/MXAConfiguration.h" HAVE_TIME_GETTIMEOFDAY)
    CHECK_SYMBOL_EXISTS( MXA_HAVE_SYS_TIME_GETTIMEOFDAY "MXA/MXAConfiguration.h" HAVE_SYS_TIME_GETTIMEOFDAY)
    # Restore CMAKE_REQUIRED_INCLUDES and CMAKE_REQUIRED_FLAGS variables
    SET(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVE})
    SET(CMAKE_REQUIRED_FLAGS    ${CMAKE_REQUIRED_FLAGS_SAVE})
    
    if ( HAVE_TIME_GETTIMEOFDAY )
      set ( VERSION_COMPILE_FLAGS "-DHAVE_TIME_GETTIMEOFDAY")
    endif()
    
    if ( HAVE_SYS_TIME_GETTIMEOFDAY )
        set ( VERSION_COMPILE_FLAGS "-DHAVE_SYS_TIME_GETTIMEOFDAY")
    endif()
    
    if (NOT HAVE_TIME_GETTIMEOFDAY AND NOT HAVE_SYS_TIME_GETTIMEOFDAY)
      set( VERSION_GEN_VER_MAJOR "0")
      set( VERSION_GEN_VER_MINOR "0")
      set( VERSION_GEN_VER_PATCH "1")
      set (VERSION_GEN_COMPLETE "0.0.1" )
      set (VERSION_GEN_NAME "${PROJECT_NAME}")
      set (VERSION_GEN_NAMESPACE "${NAMESPACE}")
      set (${PROJECT_NAME}_VERSION   ${VERSION_GEN_COMPLETE}    CACHE STRING "Complete Version String")
      set (${PROJECT_NAME}_VER_MAJOR ${VERSION_GEN_VER_MAJOR} CACHE STRING "Major Version String")
      set (${PROJECT_NAME}_VER_MINOR ${VERSION_GEN_VER_MINOR} CACHE STRING "Minor Version String")
      set (${PROJECT_NAME}_VER_PATCH ${VERSION_GEN_VER_PATCH} CACHE STRING "Patch Version String")
    else()
    
      try_run(VERSION_RUN_RESULT VERSION_COMPILE_RESULT 
              ${CMAKE_CURRENT_BINARY_DIR} ${MXADATAMODEL_RESOURCES_DIR}/CMake/GenerateVersionString.cpp
              COMPILE_DEFINITIONS ${VERSION_COMPILE_FLAGS}
              COMPILE_OUTPUT_VARIABLE VERSION_COMPILE_OUTPUT
              RUN_OUTPUT_VARIABLE VERSION_RUN_OUTPUT )
                 
      if (NOT VERSION_RUN_OUTPUT) 
          message(STATUS "VERSION_COMPILE_OUTPUT: ${VERSION_COMPILE_OUTPUT}")
          message(STATUS "VERSION_RUN_OUTPUT: ${VERSION_RUN_OUTPUT}")
          FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
              "Attempting to Generate a Version Number from a GetTimeofDay() function failed with the following output\n"
              "----------- COMPILE OUTPUT ---------------------------------------------------\n"
              "${VERSION_COMPILE_OUTPUT}\n"
              "----------- RUN OUTPUT ---------------------------------------------------\n"
              "${VERSION_RUN_OUTPUT}\n"
              "--------------------------------------------------------------\n" )
           message(FATAL_ERROR "The program to generate a version was not able to be run. Are we cross compiling? Do we have the GetTimeOfDay() function?")
            
      endif()
      # and now the version string given by qmake
      STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" VERSION_GEN_VER_MAJOR "${VERSION_RUN_OUTPUT}")
      STRING(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]+.*" "\\1" VERSION_GEN_VER_MINOR "${VERSION_RUN_OUTPUT}")
      STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_GEN_VER_PATCH "${VERSION_RUN_OUTPUT}")

      set (VERSION_GEN_COMPLETE ${VERSION_RUN_OUTPUT} )
      set (VERSION_GEN_NAME "${PROJECT_NAME}")
      set (VERSION_GEN_NAMESPACE "${NAMESPACE}")
      set (${PROJECT_NAME}_VERSION   ${VERSION_RUN_OUTPUT}    CACHE STRING "Complete Version String")
      set (${PROJECT_NAME}_VER_MAJOR ${VERSION_GEN_VER_MAJOR} CACHE STRING "Major Version String")
      set (${PROJECT_NAME}_VER_MINOR ${VERSION_GEN_VER_MINOR} CACHE STRING "Minor Version String")
      set (${PROJECT_NAME}_VER_PATCH ${VERSION_GEN_VER_PATCH} CACHE STRING "Patch Version String")

#    MESSAGE(STATUS "${PROJECT_NAME}_VERSION: ${${PROJECT_NAME}_VERSION}")
#    message(STATUS "VERSION_RUN_OUTPUT: ${VERSION_RUN_OUTPUT}")
    endif()
    configure_file(${MXADATAMODEL_RESOURCES_DIR}/CMake/Version.h.in   ${GENERATED_FILE_PATH}  )
    MARK_AS_ADVANCED(${PROJECT_NAME}_VERSION ${PROJECT_NAME}_VER_MAJOR ${PROJECT_NAME}_VER_MINOR ${PROJECT_NAME}_VER_PATCH)
endmacro()



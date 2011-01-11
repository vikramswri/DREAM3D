# -------- Function to build OS X Stand Alone Bundles -----------------
macro(MakeOSXBundleApp target binary_dir osx_tools_dir)
    set(target ${target})
    set(binary_dir ${binary_dir})
    set(osx_tools_dir ${osx_tools_dir})
    
    set (OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT "${binary_dir}/OSX_Scripts/${target}_OSX_MakeStandAloneBundle.cmake")
    
    CONFIGURE_FILE("${osx_tools_dir}/CompleteBundle.cmake.in"
            "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" @ONLY IMMEDIATE)
     
    install(SCRIPT "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}")
                    
endmacro(MakeOSXBundleApp)


# -------- Function to build OS X Stand Alone Bundles -----------------
macro(MakeOSXTool target binary_dir osx_tools_dir)
    set(target ${target})
    set(binary_dir ${binary_dir})
    set(osx_tools_dir ${osx_tools_dir})
    
    set (OSX_MAKE_STANDALONE_TOOL_CMAKE_SCRIPT 
        "${binary_dir}/OS_X_Scripts/${target}_OSX_MakeStandAloneTool.cmake")
    
    CONFIGURE_FILE("${osx_tools_dir}/CompleteTool.cmake.in"
            "${OSX_MAKE_STANDALONE_TOOL_CMAKE_SCRIPT}" @ONLY IMMEDIATE)
     
    install(SCRIPT "${OSX_MAKE_STANDALONE_TOOL_CMAKE_SCRIPT}")
                    
endmacro(MakeOSXTool)




#-------------------------------------------------------------------------------
# This macro will set all the variables necessary to have a "good" OS X Application
# bundle. The variables are as follows:
#  PROJECT_NAME - which can be taken from the ${PROJECT_NAME} variable is needed
#  DEBUG_EXTENSION - The extension used to denote a debug built Application. Typically
#   this is '_debug'
#  ICON_FILE_PATH - The complete path to the bundle icon file
#  VERSION_STRING - The version string that you wish to use for the bundle. For OS X
#   this string is usually XXXX.YY.ZZ in type. Look at the Apple docs for more info
#-------------------------------------------------------------------------------
macro(ConfigureMacOSXBundlePlist PROJECT_NAME DEBUG_EXTENSION ICON_FILE_PATH VERSION_STRING)
  message(STATUS "ConfigureMacOSXBundlePlist for ${PROJECT_NAME} ")
  IF(CMAKE_BUILD_TYPE MATCHES "Release")
    SET(DBG_EXTENSION "")
  else()
    set(DBG_EXTENSION ${DEBUG_EXTENSION})
  endif()
  get_filename_component(ICON_FILE_NAME "${ICON_FILE_PATH}" NAME)
    
 #CFBundleGetInfoString
 SET(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME}${DBG_EXTENSION} Version ${VERSION_STRING}, Copyright 2009 BlueQuartz Software.")
 SET(MACOSX_BUNDLE_ICON_FILE ${ICON_FILE_NAME})
 SET(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_NAME}${DBG_EXTENSION}")
 #CFBundleLongVersionString
 SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME}${DBG_EXTENSION} Version ${VERSION_STRING}")
 SET(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}${DBG_EXTENSION})
 SET(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION_STRING})
 SET(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION_STRING})
 SET(MACOSX_BUNDLE_COPYRIGHT "Copyright 2010, BlueQuartz Software. All Rights Reserved.")
 
 SET(${PROJECT_NAME}_PROJECT_SRCS ${${PROJECT_NAME}_PROJECT_SRCS} ${ICON_FILE_PATH})
 SET_SOURCE_FILES_PROPERTIES(${ICON_FILE_PATH} PROPERTIES
                             MACOSX_PACKAGE_LOCATION Resources)
                             
endmacro()


# --------------------------------------------------------------------
#
#
include (CMakeParseArguments)
function(BuildQtAppBundle)
    set(options )
    set(oneValueArgs TARGET DEBUG_EXTENSION ICON_FILE VERSION_MAJOR VERSION_MINOR VERSION_PATCH 
                     BINARY_DIR COMPONENT INSTALL_DEST )
    set(multiValueArgs SOURCES LINK_LIBRARIES LIB_SEARCH_DIRS QT_PLUGINS OTHER_PLUGINS)
    cmake_parse_arguments(QAB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
    
    message(STATUS "QAB_TARGET: ${QAB_TARGET}")
    
    
    #-- Configure the OS X Bundle Plist
    ConfigureMacOSXBundlePlist( ${QAB_TARGET} ${QAB_DEBUG_EXTENSION} ${QAB_ICON_FILE} 
                                "${QAB_VERSION_MAJOR}.${QAB_VERSION_MINOR}.${QAB_VERSION_PATCH}" )
                              
#-- Make sure the qt_menu.nib is copied if we are using Qt Cocoa by setting the
# source files properties of the qt_menu.nib package
    IF(QT_MAC_USE_COCOA)
        GET_FILENAME_COMPONENT(qt_menu_nib
          "${QT_QTGUI_LIBRARY_RELEASE}/Resources/qt_menu.nib"
          REALPATH)
        set(qt_menu_nib_sources
          "${qt_menu_nib}/classes.nib"
          "${qt_menu_nib}/info.nib"
          "${qt_menu_nib}/keyedobjects.nib"
          )
        SET_SOURCE_FILES_PROPERTIES(
          ${qt_menu_nib_sources}
          PROPERTIES
          MACOSX_PACKAGE_LOCATION Resources/qt_menu.nib
        )
    ELSE(QT_MAC_USE_COCOA)
        set(qt_menu_nib_sources)
    ENDIF(QT_MAC_USE_COCOA)
    
#-- Write out a qt.conf file to place in our App bundle
    set(qt_conf_file ${${QAB_TARGET}_BINARY_DIR}/qt.conf)
    file(WRITE ${qt_conf_file})
    set_source_files_properties(${qt_conf_file}
                                PROPERTIES
                                MACOSX_PACKAGE_LOCATION Resources)

#-- Add and Link our executable    
    ADD_EXECUTABLE( ${QAB_TARGET} MACOSX_BUNDLE ${QAB_SOURCES} ${QAB_ICON_FILE} 
                                        ${qt_menu_nib_sources} ${qt_conf_file} )
    TARGET_LINK_LIBRARIES( ${QAB_TARGET}
                        ${QAB_LINK_LIBRARIES} )
                        
#-- Make sure we have a proper bundle icon
    get_filename_component(ICON_FILE_NAME "${QAB_ICON_FILE}" NAME)
    SET_TARGET_PROPERTIES(${QAB_TARGET} PROPERTIES
                        MACOSX_BUNDLE_ICON_FILE ${ICON_FILE_NAME})     
        
#-- Set the Debug Suffix for the application
    SET_TARGET_PROPERTIES( ${QAB_TARGET} 
                PROPERTIES
                DEBUG_OUTPUT_NAME ${QAB_TARGET}${QAB_DEBUG_EXTENSION}
                RELEASE_OUTPUT_NAME ${QAB_TARGET}
    )

#-- Create an Install Rule for the main app bundle target
    INSTALL(TARGETS ${QAB_TARGET}
        COMPONENT ${QAB_COMPONENT}
        RUNTIME DESTINATION ${QAB_INSTALL_DEST}
        LIBRARY DESTINATION ${QAB_INSTALL_DEST} 
        ARCHIVE DESTINATION ${QAB_INSTALL_DEST}        
        BUNDLE DESTINATION ${QAB_INSTALL_DEST}
    )

#-- Create install rules for any Qt Plugins that are needed
    set(pi_dest ${QAB_TARGET}.app)
    set (target ${QAB_TARGET})
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(pi_dest ${QAB_TARGET}${QAB_DEBUG_EXTENSION}.app)
        set (target ${QAB_TARGET}${QAB_DEBUG_EXTENSION})
    endif()
    
    set(app_plugin_list "")
    set(lib_search_dirs "")
#-- It is important as you build up the list to modify the path to the Qt Plugin
#-- to point to the plugin that will appear in the Application bundle and NOT
#-- the path to your Qt installation. If you do NOT do this step properly AND you
#-- have write privs on your Qt Installation CMake will most likely "fixup" your
#-- Qt installation files which really isn't good at all. Also when generating the
#-- list it is important to have Absolute Paths to these plugins otherwise
#-- fixup_bundle() can not find the libraries.
    foreach(pi ${QAB_QT_PLUGINS})
        get_filename_component(qt_plugin_name "${pi}" NAME)
        get_filename_component(qt_plugin_type_path "${pi}" PATH)
        get_filename_component(qt_plugin_type "${qt_plugin_type_path}" NAME)
        install(PROGRAMS ${pi}
                DESTINATION "${pi_dest}/Contents/plugins/${qt_plugin_type}"
        )
        list(APPEND app_plugin_list "${CMAKE_INSTALL_PREFIX}/${pi_dest}/Contents/plugins/${qt_plugin_type}/${qt_plugin_name}")
    endforeach()
    list(REMOVE_DUPLICATES lib_search_dirs)
    
# 
#-- Create install rules for our own plugins that are targets in the build system
    foreach(pi ${QAB_OTHER_PLUGINS})
        get_filename_component(plugin_name "${pi}" NAME)
        install(PROGRAMS ${pi}
                DESTINATION "${pi_dest}/Contents/plugins"
        )
        list(APPEND app_plugin_list "${CMAKE_INSTALL_PREFIX}/${pi_dest}/Contents/plugins/${plugin_name}")
    endforeach()
    
    
#-- Create last install rule that will run fixup_bundle()
    # These variables are needed for the file to be correctly configured
    list(APPEND lib_search_dirs "${QAB_LIB_SEARCH_DIRS}")
    
    set (OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT "${QAB_BINARY_DIR}/OSX_Scripts/${QAB_TARGET}_CompleteBundle.cmake")
    
    CONFIGURE_FILE("${CMP_OSX_TOOLS_SOURCE_DIR}/CompleteBundle.cmake.in"
            "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" @ONLY IMMEDIATE)
     
    install(SCRIPT "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}")
 
endfunction()


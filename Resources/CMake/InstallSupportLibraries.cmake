#///////////////////////////////////////////////////////////////////////////////
#//
#//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#//  All rights reserved.
#//  BSD License: http://www.opensource.org/licenses/bsd-license.html
#//
#///////////////////////////////////////////////////////////////////////////////

# --------------------------------------------------------------------
# Copy the needed Qt libraries into the Build directory. Also add installation
# and CPack code to support installer generation.
# --------------------------------------------------------------------
if ( NOT Q_WS_MAC)
    if (DEFINED QT_QMAKE_EXECUTABLE)
        SET (QTLIBLIST QtCore QtGui)
        
        IF (MSVC)
            set(TYPE "d")
            FOREACH(qtlib ${QTLIBLIST})
              IF (WIN32)
                GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
                file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
                file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
                file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel)
                file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo)
                INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}${type}d4.dll 
                    DESTINATION ./
                    CONFIGURATIONS Debug
                    COMPONENT Applications)
                INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll 
                    DESTINATION ./
                    CONFIGURATIONS Release
                    COMPONENT Applications)
                add_custom_target(${qtlib}-Debug-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}${TYPE}4.dll
                            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/ 
                            COMMENT "Copying ${qtlib}${TYPE}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/")
                add_custom_target(${qtlib}-Release-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}4.dll
                            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/ 
                            COMMENT "Copying ${qtlib}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/")
              ENDIF (WIN32)
            ENDFOREACH(qtlib)
                
        endif()
    endif(DEFINED QT_QMAKE_EXECUTABLE)
endif()

# Install MinGW support libraries if those are needed
IF (MINGW)
    INCLUDE ( ${PROJECT_RESOURCES_DIR}/CMake/SHPFindMinGW.cmake )
    IF (MINGW_FOUND)
        INSTALL(FILES ${MINGW_LIBRARIES}
                DESTINATION ./
                COMPONENT Applications)
        add_custom_target(MINGW-Copy  ALL
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MINGW_LIBRARY}
                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/ 
                        COMMENT "Copying ${MINGW_LIBRARY} to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/")
    ENDIF (MINGW_FOUND)
    
   if (DEFINED QT_QMAKE_EXECUTABLE)
        SET (QTLIBLIST QtCore QtGui)
        set(TYPE "")
        IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(TYPE "d")
        endif()
        FOREACH(qtlib ${QTLIBLIST})
          IF (WIN32)
            GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp ${QT_QMAKE_EXECUTABLE} PATH)
            INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}${type}d4.dll 
                DESTINATION ./
                CONFIGURATIONS Debug
                COMPONENT Applications)
            INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll 
                DESTINATION ./
                CONFIGURATIONS Release
                COMPONENT Applications)
            add_custom_target(${qtlib}-Copy ALL
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}${TYPE}4.dll
                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/ 
                        COMMENT "Copying ${qtlib}${TYPE}4.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/")

          ENDIF (WIN32)
        ENDFOREACH(qtlib)
    endif(DEFINED QT_QMAKE_EXECUTABLE)
    
ENDIF (MINGW)



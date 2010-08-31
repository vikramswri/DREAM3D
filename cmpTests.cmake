


get_filename_component(CMP_SOURCE_DIR ${CMAKE_CURRENT_LIST_FILE} PATH CACHE)
set(CMP_CONFIGURED_FILES_SOURCE_DIR ${CMP_SOURCE_DIR}/ConfiguredFiles CACHE INTERNAL "")
set(CMP_CORE_TESTS_SOURCE_DIR ${CMP_SOURCE_DIR}/CoreTests CACHE INTERNAL "")
set(CMP_INSTALLATION_SUPPORT_SOURCE_DIR ${CMP_SOURCE_DIR}/InstallationSupport CACHE INTERNAL "")
set(CMP_MODULES_SOURCE_DIR ${CMP_SOURCE_DIR}/Modules CACHE INTERNAL "")
set(CMP_OSX_TOOLS_SOURCE_DIR ${CMP_SOURCE_DIR}/OSX_TOOLS CACHE INTERNAL "")

INCLUDE (${PROJECT_RESOURCES_DIR}/cmp/cmpCMakeMacros.cmake )

include ( ${CMP_CORE_TESTS_SOURCE_DIR}/cmpConfigureChecks.cmake )
if (NOT DEFINED CMP_PROJECT_NAMESPACE)
    set(CMP_PROJECT_NAMESPACE "CMP")
endif()

if (NOT DEFINED CMP_HEADER_DIR)
    set (CMP_HEADER_DIR ${PROJECT_BINARY_DIR}/cmp)
endif()

if (NOT DEFINED CMP_CONFIGURATION_FILE_NAME)
    set (CMP_CONFIGURATION_FILE_NAME "cmpConfiguration.h")
endif()

if (NOT DEFINED CMP_TYPES_FILE_NAME)
    set (CMP_TYPES_FILE_NAME "cmpTypes.h")
endif()

if (NOT DEFINED CMP_VERSION_HEADER_FILE_NAME)
    set (CMP_VERSION_HEADER_FILE_NAME "cmpVersion.h")
endif()

if (NOT DEFINED CMP_EXTRA_CONFIGURATION_FILE)
    set (CMP_EXTRA_CONFIGURATION_FILE "")
endif()

if (NOT DEFINED CMP_PROJECT_NAME)
    set (CMP_PROJECT_NAME "CMP")
endif()

#message(STATUS "CMP_HEADER_DIR: ${CMP_HEADER_DIR}")
#message(STATUS "CMP_CONFIGURATION_FILE_NAME: ${CMP_CONFIGURATION_FILE_NAME}")
#message(STATUS "CMP_TYPES_FILE_NAME: ${CMP_TYPES_FILE_NAME}")

get_filename_component(CMP_CONFIGURATION_HEADER_GUARD ${CMP_CONFIGURATION_FILE_NAME} NAME_WE)
get_filename_component(CMP_TYPES_HEADER_GUARD ${CMP_TYPES_FILE_NAME} NAME_WE)
get_filename_component(CMP_VERSION_HEADER_GUARD ${CMP_VERSION_HEADER_FILE_NAME} NAME_WE)

# --------------------------------------------------------------------
# Generate our header files
# --------------------------------------------------------------------
configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpConfiguration.h.in
               ${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME} )
configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpPrimitiveTypes.h.in
               ${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME} )

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
cmpGenerateVersionString( "${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}" 
                          "${CMP_PROJECT_NAMESPACE}" 
                          "${CMP_PROJECT_NAME}")

cmp_IDE_GENERATED_PROPERTIES( "Generated" 
"${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME}" 
"${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME}" 
"${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}")  

   
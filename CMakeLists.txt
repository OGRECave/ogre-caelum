######################################################################
# Caelum BUILD SYSTEM
# Welcome to the CMake build system for Caelum
# This is the main file where we prepare the general build environment
# and provide build configuration options.
######################################################################
# cmake system for Caelum updated on 2-9-2021 by Edgar{at}AnotherFoxGuy{DOT}com

cmake_minimum_required(VERSION 3.0.2)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# define the project
project(
        Caelum
        HOMEPAGE_URL https://ogrecave.github.io/ogre-caelum/
        DESCRIPTION "Caelum is an add-on for the 3D graphics rendering engine OGRE, aimed to render atmospheric effects."
        VERSION 0.6.4
)

# add some functions we use that are shipped with cmake
include(CheckLibraryExists)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckCCompilerFlag)
include(CheckCSourceCompiles)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

option(CAELUM_OGRE_PLUGIN "Build Caelum as OGRE plugin" ON)
option(CAELUM_SCRIPT_SUPPORT "Load CaelumSystem and it's components from a script file" ON)
option(CAELUM_BUILD_SAMPLES "build samples" OFF)

# build static libs by default
SET(BUILD_SHARED_LIBS OFF)

SET(CMAKE_USE_RELATIVE_PATHS OFF)

find_package(OGRE REQUIRED)
link_directories(${OGRE_LIBRARY_DIRS})

# setup paths
SET(RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")
SET(LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/")
SET(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/")
SET(EXECUTABLE_OUTPUT_PATH ${RUNTIME_OUTPUT_DIRECTORY})
SET(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_DIRECTORY})
SET(CMAKEFILES_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/Caelum")

# configuration of the config.h and PkgConfig
configure_file("${CMAKE_SOURCE_DIR}/main/include/CaelumConfig.h.in" "${CMAKE_BINARY_DIR}/main/include/CaelumConfig.h")

# some additional compiler flags
if (NOT WIN32)
    ADD_DEFINITIONS(-Wall)
    CHECK_C_COMPILER_FLAG(-Wextra HAVE_W_EXTRA)
    if (HAVE_W_EXTRA)
        ADD_DEFINITIONS(-Wextra -Wno-unused-parameter)
    endif ()
endif ()

# Set visibility options if available
#IF(NOT WIN32)
#	CHECK_C_SOURCE_COMPILES("int foo() __attribute__((destructor));
#								int main() {return 0;}" HAVE_GCC_DESTRUCTOR)
#
#	CHECK_C_COMPILER_FLAG(-fvisibility=hidden HAVE_VISIBILITY_SWITCH)
#	IF(HAVE_VISIBILITY_SWITCH)
#		CHECK_C_SOURCE_COMPILES("int foo() __attribute__((visibility(\"default\")));
#									int main() {return 0;}" HAVE_GCC_VISIBILITY)
#		IF(HAVE_GCC_VISIBILITY)
#			ADD_DEFINITIONS(-fvisibility=hidden -DHAVE_GCC_VISIBILITY)
#		ENDIF()
#	ENDIF()
#ENDIF()

set(LIB_DESTINATION "lib")

# now add the directories
add_subdirectory(main)

if (CAELUM_BUILD_SAMPLES)
    add_subdirectory(samples)
endif ()

# doxygen stuff
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # prepare doxygen configuration file
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    add_custom_target(
            doc_doxygen
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating documentation with Doxygen."
            VERBATIM
    )

    # cleanup $build/doc on "make clean"
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES doc)

    # install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html/ DESTINATION doc/caelum/api)
    # install man pages into packages, scope is now project root..
    #install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api-doc/man/man3 DESTINATION share/man/man3/ )
endif (DOXYGEN_FOUND)

# other doc files
set(DOC_FILES Contributors.txt LICENSE ReadMe.md)
install(FILES ${DOC_FILES} DESTINATION TYPE DOC)

# install the PkgConfig file
configure_file("${CMAKE_SOURCE_DIR}/Caelum.pc.in" "${CMAKE_BINARY_DIR}/Caelum.pc" @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/Caelum.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

configure_package_config_file(
        CaelumConfig.cmake.in
        ${CMAKE_BINARY_DIR}/CaelumConfig.cmake
        INSTALL_DESTINATION ${CMAKEFILES_INSTALL_DIR}
)

write_basic_package_version_file(
        ${CMAKE_BINARY_DIR}/CaelumConfigVersion.cmake
        VERSION "${CMAKE_PROJECT_VERSION}"
        COMPATIBILITY SameMajorVersion
)

install(
        FILES ${CMAKE_BINARY_DIR}/CaelumConfig.cmake
        ${CMAKE_BINARY_DIR}/CaelumConfigVersion.cmake
        DESTINATION ${CMAKEFILES_INSTALL_DIR}
)
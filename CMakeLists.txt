######################################################################
# Caelum BUILD SYSTEM
# Welcome to the CMake build system for Caelum
# This is the main file where we prepare the general build environment
# and provide build configuration options.
######################################################################
# cmake system for Caelum updated on 6-10-2017 by Edgar{at}AnotherFoxGuy{DOT}com

cmake_minimum_required(VERSION 3.0.2)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)


# add some functions we use that are shipped with cmake
INCLUDE(CheckLibraryExists)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckIncludeFileCXX)
INCLUDE(CheckCCompilerFlag)
INCLUDE(CheckCSourceCompiles)

# define the project
project(Caelum VERSION 0.6.3)

# some versioning things
SET(LIB_MAJOR_VERSION ${Caelum_VERSION_MAJOR})
SET(LIB_MINOR_VERSION ${Caelum_VERSION_MINOR})
SET(LIB_BUILD_VERSION ${Caelum_VERSION_PATCH})

option( CAELUM_OGRE_PLUGIN "Build Caelum as OGRE plugin" ON )
option( CAELUM_SCRIPT_SUPPORT "Load CaelumSystem and it's components from a script file" ON)
option( CAELUM_BUILD_SAMPLES "build samples" OFF)

# build static libs by default
SET(BUILD_SHARED_LIBS OFF)

SET(CMAKE_USE_RELATIVE_PATHS OFF)

find_package(OGRE REQUIRED CONFIG)
link_directories(${OGRE_LIBRARY_DIRS})

# setup paths
SET(RUNTIME_OUTPUT_DIRECTORY "${Caelum_SOURCE_DIR}/bin/")
SET(LIBRARY_OUTPUT_DIRECTORY "${Caelum_SOURCE_DIR}/lib/")
SET(ARCHIVE_OUTPUT_DIRECTORY "${Caelum_SOURCE_DIR}/lib/")
SET(EXECUTABLE_OUTPUT_PATH ${RUNTIME_OUTPUT_DIRECTORY})
SET(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_DIRECTORY})

# Needed for Caelum.pc.in
SET(prefix ${CMAKE_INSTALL_PREFIX})
SET(exec_prefix "\${prefix}")
SET(libdir "${OGRE_PLUGIN_DIR}")
SET(bindir "\${exec_prefix}/bin")
SET(includedir "\${prefix}/include")
SET(PACKAGE_NAME "Caelum")
SET(PACKAGE_VERSION "${LIB_VERSION}")

# configuration of the config.h and PkgConfig
CONFIGURE_FILE(
        "${Caelum_SOURCE_DIR}/main/include/CaelumConfig.h.in"
        "${Caelum_BINARY_DIR}/main/include/CaelumConfig.h")
CONFIGURE_FILE(
        "${Caelum_SOURCE_DIR}/Caelum.pc.in"
        "${Caelum_BINARY_DIR}/Caelum.pc"
        @ONLY)
# install the PkgConfig file
INSTALL(FILES "${Caelum_BINARY_DIR}/Caelum.pc" DESTINATION "lib/pkgconfig")

# some additional compiler flags
IF (NOT WIN32)
    ADD_DEFINITIONS(-Wall)
    CHECK_C_COMPILER_FLAG(-Wextra HAVE_W_EXTRA)
    IF (HAVE_W_EXTRA)
        ADD_DEFINITIONS(-Wextra -Wno-unused-parameter)
    ENDIF ()
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
set(LIBNAME "Caelum")

# now add the directories
add_subdirectory(main)

if(CAELUM_BUILD_SAMPLES)
	add_subdirectory(samples)
endif()


# cpack
set(CPACK_PACKAGE_DESCRIPTION "Caelum")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Caelum is an add-on for the 3D graphics rendering engine OGRE, aimed to render atmospheric effects.")
set(CPACK_PACKAGE_NAME "caelum")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libogre-1.9-dev")
set(CPACK_PACKAGE_CONTACT "Edgar@AnotherFoxGuy.com")
set(CPACK_PACKAGE_VENDOR "Edgar@AnotherFoxGuy.com")
SET(CPACK_PACKAGE_VERSION ${LIB_VERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR ${LIB_MAJOR_VERSION})
SET(CPACK_PACKAGE_VERSION_MINOR ${LIB_MINOR_VERSION})
SET(CPACK_PACKAGE_VERSION_PATCH ${LIB_BUILD_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.md")

SET(CPACK_GENERATOR ZIP)
IF (UNIX)
    SET(CPACK_GENERATOR ${CPACK_GENERATOR};STGZ;TGZ;DEB;RPM)
ENDIF (UNIX)
IF (LINUX)
    SET(CPACK_GENERATOR ${CPACK_GENERATOR};DEB;RPM)
ENDIF (LINUX)
IF (MSVC)
    SET(CPACK_GENERATOR ${CPACK_GENERATOR};NSIS)
ENDIF (MSVC)
IF (APPLE)
    SET(CPACK_GENERATOR ${CPACK_GENERATOR};PackageMaker)
ENDIF (APPLE)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${LIB_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
include(CPack)

# doxygen stuff
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # prepare doxygen configuration file
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    add_custom_target(doc-doxygen
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating documentation with Doxygen."
            VERBATIM
            )

    # cleanup $build/api-doc on "make clean"
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES api-doc)

    # install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html/ DESTINATION doc/caelum/api)
    # install man pages into packages, scope is now project root..
    #install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api-doc/man/man3 DESTINATION share/man/man3/ )
endif (DOXYGEN_FOUND)

# other doc files
set(DOC_FILES Contributors.txt LICENSE ReadMe.md)
install(FILES ${DOC_FILES} DESTINATION doc/caelum/)

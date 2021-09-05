FILE(GLOB sources ${CMAKE_SOURCE_DIR}/main/src/*.cpp ${CMAKE_SOURCE_DIR}/main/include/*.h)

add_definitions("-DCAELUM_LIB")

add_library(${CMAKE_PROJECT_NAME} SHARED ${sources})

target_include_directories(
        ${CMAKE_PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/main/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/main/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Caelum>
)

if (UNIX)
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES VERSION ${OGRE_VERSION})
endif ()

# install the library
install(
        TARGETS ${CMAKE_PROJECT_NAME}
        EXPORT CaelumTargets
        RUNTIME LIBRARY ARCHIVE
)

install(
        EXPORT CaelumTargets
        FILE CaelumTargets.cmake
        NAMESPACE Caelum::
        DESTINATION ${CMAKEFILES_INSTALL_DIR}
)

# install the headers: both source and build headers
install(DIRECTORY ${CMAKE_SOURCE_DIR}/main/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Caelum FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
install(DIRECTORY ${CMAKE_BINARY_DIR}/main/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Caelum FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

# --- Ogre 3D graphics engine ---
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OgreMain)

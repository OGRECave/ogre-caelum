
configure_file(${CMAKE_SOURCE_DIR}/cmake/resources.cfg.in ${CMAKE_SOURCE_DIR}/bin/resources.cfg)
add_executable(CaelumDemo ${CMAKE_SOURCE_DIR}/samples/src/CaelumDemo.cpp)
target_link_libraries(CaelumDemo PRIVATE Caelum OgreBites)

target_include_directories(CaelumDemo PRIVATE ${CMAKE_SOURCE_DIR}/samples/include)

add_executable(CaelumTest ${CMAKE_SOURCE_DIR}/samples/src/CaelumTest.cpp)
target_link_libraries(CaelumTest PRIVATE Caelum)

# CaelumLab needs Ogre built with the ImGui overlay and a recent ImGui
get_target_property(OGRE_OVERLAY_INCLUDE_DIRS OgreOverlay INTERFACE_INCLUDE_DIRECTORIES)
find_file(OGRE_IMGUI_HEADER imgui.h PATHS ${OGRE_OVERLAY_INCLUDE_DIRS} NO_DEFAULT_PATH)
if(OGRE_IMGUI_HEADER AND OGRE_VERSION VERSION_GREATER_EQUAL 14.5)
    add_executable(CaelumLab ${CMAKE_SOURCE_DIR}/samples/src/CaelumLab.cpp)
    target_link_libraries(CaelumLab PRIVATE Caelum OgreBites OgreOverlay OgreRTShaderSystem OgreTerrain)
    target_include_directories(CaelumLab PRIVATE ${CMAKE_SOURCE_DIR}/samples/include)
else()
    message(STATUS "CaelumLab requires Ogre 14.5 or later with the ImGui overlay; skipping it")
endif()
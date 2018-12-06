include_directories(
    ${OGRE_INCLUDE_DIRS} 
    ${CMAKE_BINARY_DIR}/main/include/
    ${CMAKE_SOURCE_DIR}/main/include/
    ${CMAKE_SOURCE_DIR}/samples/include)

configure_file(${CMAKE_SOURCE_DIR}/cmake/resources.cfg.in ${CMAKE_SOURCE_DIR}/bin/resources.cfg)
add_executable(CaelumDemo ${CMAKE_SOURCE_DIR}/samples/src/CaelumDemo.cpp)
target_link_libraries(CaelumDemo ${LIBNAME} ${OGRE_LIBRARIES})

link_directories(/usr/local/lib/cegui-0.8)
add_executable(CaelumLab ${CMAKE_SOURCE_DIR}/samples/src/CaelumLab.cpp)
target_link_libraries(CaelumLab ${LIBNAME} ${OGRE_LIBRARIES} CEGUIBase-0 CEGUIOgreRenderer-0 OIS)
target_include_directories(CaelumLab PRIVATE /usr/local/include/cegui-0 /usr/local/include/cegui-0/CEGUI/RendererModules /usr/include/ois/)
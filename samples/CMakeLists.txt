include_directories(
    ${OGRE_INCLUDE_DIRS} 
    ${CMAKE_BINARY_DIR}/main/include/
    ${CMAKE_SOURCE_DIR}/main/include/
    ${CMAKE_SOURCE_DIR}/samples/include)

configure_file(${CMAKE_SOURCE_DIR}/cmake/resources.cfg.in ${CMAKE_SOURCE_DIR}/bin/resources.cfg)
add_executable(CaelumDemo ${CMAKE_SOURCE_DIR}/samples/src/CaelumDemo.cpp)
target_link_libraries(CaelumDemo Caelum ${OGRE_LIBRARIES})

# add_executable(CaelumLab ${CMAKE_SOURCE_DIR}/samples/src/CaelumLab.cpp)
# target_link_libraries(CaelumLab Caelum ${OGRE_LIBRARIES})

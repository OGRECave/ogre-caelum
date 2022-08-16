
configure_file(${CMAKE_SOURCE_DIR}/cmake/resources.cfg.in ${CMAKE_SOURCE_DIR}/bin/resources.cfg)
add_executable(CaelumDemo ${CMAKE_SOURCE_DIR}/samples/src/CaelumDemo.cpp)
target_link_libraries(CaelumDemo PRIVATE Caelum OgreBites)

target_include_directories(CaelumDemo PRIVATE ${CMAKE_SOURCE_DIR}/samples/include)

add_executable(CaelumTest ${CMAKE_SOURCE_DIR}/samples/src/CaelumTest.cpp)
target_link_libraries(CaelumTest PRIVATE Caelum)

# add_executable(CaelumLab ${CMAKE_SOURCE_DIR}/samples/src/CaelumLab.cpp)
# target_link_libraries(CaelumLab Caelum ${OGRE_LIBRARIES})
include_directories(${Ogre_INCLUDE_DIRS})
link_directories   (${Ogre_LIBRARY_DIRS})
include_directories(${Caelum_SOURCE_DIR}/main/include/)
include_directories(${Caelum_BINARY_DIR}/main/include/)

FILE(GLOB sources ${Caelum_SOURCE_DIR}/main/src/*.cpp ${Caelum_SOURCE_DIR}/main/include/*.h)

set(LIBNAME "Caelum")


find_package(Boost 1.44 COMPONENTS date_time filesystem)
include_directories(${Boost_INCLUDE_DIRS})
link_directories   (${Boost_LIBRARY_DIRS})

add_definitions("-DCAELUM_LIB -DCAELUM_SCRIPT_SUPPORT -D_CAELUM_TYPE_DESCRIPTORS")

ADD_LIBRARY(${LIBNAME} SHARED ${sources})

target_link_libraries(${LIBNAME} ${Ogre_LIBRARIES})

# install the library
INSTALL(TARGETS ${LIBNAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
)

# install the headers: both source and build headers
INSTALL(DIRECTORY ${Caelum_SOURCE_DIR}/main/include/ DESTINATION include/Caelum FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
INSTALL(DIRECTORY ${Caelum_BINARY_DIR}/main/include/ DESTINATION include/Caelum FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

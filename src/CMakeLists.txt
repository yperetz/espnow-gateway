# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.


FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.cpp
                              ${CMAKE_SOURCE_DIR}/utils/*.cpp)

FILE(GLOB app_include_dirs "." ${CMAKE_SOURCE_DIR}/src/include
                               ${CMAKE_SOURCE_DIR}/utils/include
                               ${CMAKE_SOURCE_DIR}/config/include)

idf_component_register(SRCS ${app_sources}
                       INCLUDE_DIRS ${app_include_dirs})

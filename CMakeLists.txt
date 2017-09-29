cmake_minimum_required(VERSION 2.8)

project(TreeWrapper LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(ROOT REQUIRED)

include_directories(${ROOT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/interface)

add_library(TreeWrapper SHARED src/Brancher.cc src/Leaf.cc src/TreeGroup.cc src/TreeWrapperAccessor.cc src/TreeWrapper.cc)
target_link_libraries(TreeWrapper ${ROOT_LIBRARIES})

install(TARGETS TreeWrapper LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY interface/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

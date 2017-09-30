cmake_minimum_required(VERSION 2.8)

project(TreeWrapper LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(ROOT REQUIRED)
find_package(Boost REQUIRED)
message(STATUS "Found Boost with include dir ${Boost_INCLUDE_DIR} (change by passing e.g. '-DBoost_NO_BOOST_CMAKE=ON -DBOOST_ROOT=$(scram tool tag boost BOOST_BASE)')")

include_directories(${ROOT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/interface)

add_library(TreeWrapper SHARED src/Brancher.cc src/Leaf.cc src/TreeGroup.cc src/TreeWrapperAccessor.cc src/TreeWrapper.cc)
target_link_libraries(TreeWrapper ${ROOT_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS TreeWrapper LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY interface/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

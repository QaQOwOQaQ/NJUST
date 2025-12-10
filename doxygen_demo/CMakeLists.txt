cmake_minimum_required(VERSION 3.16)
project(complex_docs CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app
  src/main.cpp
  src/logger.cpp
  src/reader.cpp
  src/node.cpp
)
target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/include)

find_package(Doxygen REQUIRED)

set(DOXYGEN_IN  ${CMAKE_SOURCE_DIR}/Doxyfile)
set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)
configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

add_custom_target(doc
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Generating Doxygen docs (with Graphviz)"
  VERBATIM
)

cmake_minimum_required(VERSION 3.9)
project("explicit_instantiation")
# SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(explicit_instantiation
       "main.cpp" "test_function_template.cpp" "test_class_template.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET explicit_instantiation PROPERTY CXX_STANDARD 20)
endif()
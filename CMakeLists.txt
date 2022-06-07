project(exemplo-boost-serialization)
cmake_minimum_required(VERSION 3.8)

include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup()

add_executable(example main.cpp)

set_property(TARGET example PROPERTY CXX_STANDARD 17)
set_property(TARGET example PROPERTY CXX_STANDARD_REQUIRED TRUE)

target_include_directories(example
  SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/deps/eos-portable-archive")
target_link_libraries(example ${CONAN_LIBS})

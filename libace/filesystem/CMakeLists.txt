set(CMAKE_POSITION_INDEPENDENT_CODE 1)
file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)
add_library(ace_filesystem OBJECT ${SOURCES})
target_compile_features(ace_filesystem PRIVATE cxx_nullptr)

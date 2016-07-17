set(CMAKE_POSITION_INDEPENDENT_CODE 1)
file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)
add_library(ace_model OBJECT ${SOURCES})
target_compile_features(ace_model PRIVATE cxx_nullptr)

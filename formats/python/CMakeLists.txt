include_directories(${Python3_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_library(ace_python_format SHARED  Array.cpp
                                      Common.cpp
                                      Object.cpp
                                      Primitive.cpp
                                      Scanner.cpp)

target_compile_features(ace_python_format PRIVATE cxx_nullptr)

target_link_libraries(ace_python_format
  PRIVATE ace
  PUBLIC ${Python3_LIBRARIES})

set_target_properties(ace_python_format
  PROPERTIES
  VERSION ${LIB_VERSION}
  SOVERSION ${LIB_SOVERSION})

install(
  TARGETS ace_python_format
  LIBRARY DESTINATION lib)

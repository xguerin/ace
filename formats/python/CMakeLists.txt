include_directories(${PYTHON_INCLUDE_DIR})

add_library(ace_python_format SHARED  Array.cpp
                                      Common.cpp
                                      Object.cpp
                                      Primitive.cpp
                                      Scanner.cpp)

target_compile_features(ace_python_format PRIVATE cxx_nullptr)
target_link_libraries(ace_python_format PRIVATE ace python2.7)

install(
  TARGETS ace_python_format
  LIBRARY DESTINATION lib
  )

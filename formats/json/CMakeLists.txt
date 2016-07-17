add_library(ace_json_format SHARED  Array.cpp
                                    Common.cpp
                                    Object.cpp
                                    Primitive.cpp
                                    Scanner.cpp)

target_compile_features(ace_json_format PRIVATE cxx_nullptr)
target_link_libraries(ace_json_format PRIVATE ace jansson)

install(
  TARGETS ace_json_format
  LIBRARY DESTINATION lib
  )

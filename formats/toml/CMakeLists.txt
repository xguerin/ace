add_library(ace_toml_format SHARED  Array.cpp
                                    Common.cpp
                                    Object.cpp
                                    Primitive.cpp
                                    Scanner.cpp)

target_compile_features(ace_toml_format PRIVATE cxx_nullptr)

target_link_libraries(ace_toml_format
  PRIVATE ace)

set_target_properties(ace_toml_format
  PROPERTIES
  VERSION ${LIB_VERSION}
  SOVERSION ${LIB_SOVERSION})

install(
  TARGETS ace_toml_format
  LIBRARY DESTINATION lib)

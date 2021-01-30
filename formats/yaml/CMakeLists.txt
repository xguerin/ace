include_directories(${YAMLCPP_INCLUDE_DIR})

add_library(ace_yaml_format SHARED Array.cpp
                                   Common.cpp
                                   Object.cpp
                                   Primitive.cpp
                                   Scanner.cpp)

target_compile_features(ace_yaml_format PRIVATE cxx_nullptr)
target_link_libraries(ace_yaml_format PRIVATE ace PUBLIC ${YAMLCPP_LIBRARY})

set_target_properties(ace_yaml_format
  PROPERTIES
  VERSION ${LIB_VERSION}
  SOVERSION ${LIB_SOVERSION})

install(
  TARGETS ace_yaml_format
  LIBRARY DESTINATION lib)

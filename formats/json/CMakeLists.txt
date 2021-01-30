include_directories(${JANSSON_INCLUDE_DIR})

add_library(ace_json_format SHARED  Array.cpp
                                    Common.cpp
                                    Object.cpp
                                    Primitive.cpp
                                    Scanner.cpp)

target_compile_features(ace_json_format PRIVATE cxx_nullptr)

target_link_libraries(ace_json_format
  PRIVATE ace
  PUBLIC ${JANSSON_LIBRARY})

set_target_properties(ace_json_format
  PROPERTIES
  VERSION ${LIB_VERSION}
  SOVERSION ${LIB_SOVERSION})

install(
  TARGETS ace_json_format
  LIBRARY DESTINATION lib)

include_directories(${HJSON_INCLUDE_DIR})

file(GLOB SOURCES *.cpp *.h)
add_library(ace_hjson_format SHARED ${SOURCES})

target_compile_features(ace_hjson_format PRIVATE cxx_nullptr)

target_link_libraries(ace_hjson_format
  PRIVATE ace
  PUBLIC ${HJSON_LIBRARY})

set_target_properties(ace_hjson_format
  PROPERTIES
  VERSION ${LIB_VERSION}
  SOVERSION ${LIB_SOVERSION})

install(
  TARGETS ace_hjson_format
  LIBRARY DESTINATION lib)

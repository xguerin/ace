add_subdirectory(common)
add_subdirectory(engine)
add_subdirectory(filesystem)
add_subdirectory(model)
add_subdirectory(tree)
add_subdirectory(types)

add_library(ace SHARED
  $<TARGET_OBJECTS:ace_common>
  $<TARGET_OBJECTS:ace_engine>
  $<TARGET_OBJECTS:ace_filesystem>
  $<TARGET_OBJECTS:ace_model>
  $<TARGET_OBJECTS:ace_tree>
  $<TARGET_OBJECTS:ace_types>)

target_link_libraries(ace PUBLIC ${RE2_LIBRARY} ${CMAKE_DL_LIBS})

set_target_properties(ace
  PROPERTIES
  VERSION ${LIB_VERSION}
  SOVERSION ${LIB_SOVERSION})

install(
  TARGETS ace
  LIBRARY DESTINATION lib)

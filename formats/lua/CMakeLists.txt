include_directories(${LUA_INCLUDE_DIR})

add_library(ace_lua_format  SHARED  Array.cpp
                                    Common.cpp
                                    Object.cpp
                                    Primitive.cpp
                                    Scanner.cpp)

target_compile_features(ace_lua_format PRIVATE cxx_nullptr)

target_link_libraries(ace_lua_format
  PRIVATE ace
  PUBLIC ${LUA_LIBRARIES})

install(
  TARGETS ace_lua_format
  LIBRARY DESTINATION lib
  )

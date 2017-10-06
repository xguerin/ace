add_executable(ace-compile  compile.cpp)
add_executable(ace-convert  convert.cpp)
add_executable(ace-explain  explain.cpp)
add_executable(ace-path     path.cpp)
add_executable(ace-validate validate.cpp)

target_compile_features(ace-compile  PRIVATE cxx_nullptr)
target_compile_features(ace-convert  PRIVATE cxx_nullptr)
target_compile_features(ace-explain  PRIVATE cxx_nullptr)
target_compile_features(ace-path     PRIVATE cxx_nullptr)
target_compile_features(ace-validate PRIVATE cxx_nullptr)

target_link_libraries(ace-compile  PRIVATE ace)
target_link_libraries(ace-convert  PRIVATE ace)
target_link_libraries(ace-explain  PRIVATE ace)
target_link_libraries(ace-path     PRIVATE ace)
target_link_libraries(ace-validate PRIVATE ace)

install(
  TARGETS ace-compile ace-convert ace-explain ace-path ace-validate 
  RUNTIME DESTINATION bin
  )

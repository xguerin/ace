include_directories(SYSTEM ${GTEST_INCLUDE_DIR})

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

add_executable(ace-tests ${SOURCES})
add_test(gtest ace-tests)

target_compile_features(ace-tests PRIVATE cxx_nullptr)
target_link_libraries(ace-tests PRIVATE ace ${GTEST_LIBRARIES})

install(
  TARGETS ace-tests
  RUNTIME DESTINATION bin
  )

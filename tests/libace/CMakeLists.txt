include_directories(SYSTEM ${GTEST_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable -Wno-sign-compare")

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

add_executable(libace-tests ${SOURCES})
add_test(gtest libace-tests)

target_compile_features(libace-tests PRIVATE cxx_nullptr)
target_link_libraries(libace-tests PRIVATE ace PUBLIC ${GTEST_LIBRARIES})

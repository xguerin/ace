include_directories(SYSTEM ${GTEST_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable -Wno-sign-compare")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I ${CMAKE_CURRENT_BINARY_DIR}")

function(CHANGE_EXT var ext)
  set(names "")
  foreach(f ${ARGN})
    get_filename_component(FILE_WITHOUT_EXT ${f} NAME_WE)
    list(APPEND names "${FILE_WITHOUT_EXT}.${ext}")
  endforeach(f)
  set(${var} "${names}" PARENT_SCOPE)
endfunction(CHANGE_EXT)

file(GLOB MODELS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.json)
file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

CHANGE_EXT(MODEL_AC_H "ac.h" ${MODELS})
CHANGE_EXT(MODEL_AC_CPP "ac.cpp" ${MODELS})

add_custom_command(
  OUTPUT  ${MODEL_AC_H} ${MODEL_AC_CPP}
  COMMAND ${CMAKE_COMMAND} -E env ACE_SCANNER_PATH=${CMAKE_CURRENT_BINARY_DIR}/../../formats/json ${CMAKE_CURRENT_BINARY_DIR}/../../tools/ace-compile -I ${CMAKE_CURRENT_SOURCE_DIR} ${MODELS}
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/../../tools/ace-compile"
  COMMENT "Compiling test models"
  VERBATIM
  )

add_executable(codegen-tests ${SOURCES} ${MODEL_AC_CPP})
add_test(gtest codegen-tests)

target_compile_features(codegen-tests PRIVATE cxx_nullptr)
target_link_libraries(codegen-tests PRIVATE ace PUBLIC ${GTEST_LIBRARIES})

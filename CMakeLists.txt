cmake_minimum_required(VERSION 3.16)

project(ct_algo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCES "1-1/*.cpp" "1-2/*.cpp")

set(PROJECT_WARNING_FLAGS
  -Wall
  -Wextra
  -Wpedantic
  -Werror
  -Wconversion
  -Wsign-conversion
  -Wfloat-conversion
  -Wdouble-promotion
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  list(APPEND PROJECT_WARNING_FLAGS
    -Wimplicit-float-conversion
    -Wshorten-64-to-32
  )
endif()

foreach(SOURCE ${SOURCES})
  file(RELATIVE_PATH REL_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${SOURCE})
  string(REPLACE ".cpp" "" NAME_NO_EXT ${REL_PATH})
  string(REPLACE "/" "" TARGET_NAME ${NAME_NO_EXT})
  add_executable(${TARGET_NAME} ${SOURCE})
  target_compile_options(${TARGET_NAME} PRIVATE ${PROJECT_WARNING_FLAGS} -g -ggdb)
endforeach()

set(CMAKE_CXX_FLAGS_ASAN "-fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_TSAN "-fsanitize=thread -fno-omit-frame-pointer")


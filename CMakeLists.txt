cmake_minimum_required(VERSION 3.16)

project(ct_algo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCES "1-1/*.cpp" "1-2/*.cpp")

foreach(SOURCE ${SOURCES})
    file(RELATIVE_PATH REL_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${SOURCE})
    string(REPLACE ".cpp" "" NAME_NO_EXT ${REL_PATH})
    string(REPLACE "/" "" TARGET_NAME ${NAME_NO_EXT})
    add_executable(${TARGET_NAME} ${SOURCE})
endforeach()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_ASAN "-fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_TSAN "-fsanitize=thread -fno-omit-frame-pointer")

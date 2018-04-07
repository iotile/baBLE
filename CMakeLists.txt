cmake_minimum_required(VERSION 3.10)
project(baBLE_linux)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES_FILES
        src/main.cpp
        src/interfaces/SerializerInterface.h
        src/interfaces/BaBLEIOInterface.h
        src/interfaces/BluetoothIOInterface.h
        src/Log.cpp
        src/Log.h
        src/utils/colors.h)
add_executable(baBLE_linux ${SOURCES_FILES})

add_subdirectory(deps/uv)
target_link_libraries(baBLE_linux uv)

include_directories(deps/uvw)

## Install
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/baBLE_linux
        DESTINATION bin)

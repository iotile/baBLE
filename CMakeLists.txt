cmake_minimum_required(VERSION 3.10)
project(baBLE_linux)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES_FILES src/main.cpp
        src/SerializerInterface.h
        src/BaBLEIOInterface.h
        src/BluetoothIOInterface.h
    )
add_executable(baBLE_linux ${SOURCES_FILES})
add_subdirectory(deps/uv)
target_link_libraries(baBLE_linux uv)

## Install
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/baBLE_linux
        DESTINATION bin)

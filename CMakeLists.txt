cmake_minimum_required(VERSION 3.10)
project(baBLE_linux)

set(CMAKE_CXX_STANDARD 14)

## Build executable from sources files
set(SOURCES_FILES
        src/main.cpp
#        src/interfaces/BaBLEIOInterface.hpp
#        src/interfaces/BluetoothIOInterface.hpp
        src/Log.cpp
        src/Log.hpp
        src/utils/colors.hpp
        src/utils/stream_formats.hpp
        src/Serializer.cpp
        src/Serializer.hpp
        src/Deserializer.cpp
        src/Deserializer.hpp
#        src/AbstractSerializer.cpp
#        src/AbstractSerializer.hpp
        )
add_executable(baBLE_linux ${SOURCES_FILES})

## Include dependencies
### libuv
add_subdirectory(deps/uv)
target_link_libraries(baBLE_linux uv)
### uvw
include_directories(deps/uvw)

## Install
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/baBLE_linux
        DESTINATION bin)

cmake_minimum_required(VERSION 3.10)
project(baBLE_linux)

set(CMAKE_CXX_STANDARD 14)

## Build executable from sources files
set(SOURCES_FILES
        src/main.cpp
        src/utils/colors.hpp
        src/utils/stream_formats.hpp
        src/Log/Log.cpp
        src/Log/Log.hpp
        src/Log/Loggable.hpp
        src/Serializer/AbstractSerializer.cpp
        src/Serializer/AbstractSerializer.hpp
        src/Serializer/Serializer.cpp
        src/Serializer/Serializer.hpp
        src/Serializer/Deserializer.cpp
        src/Serializer/Deserializer.hpp
        src/Socket/MGMTSocket.cpp
        src/Socket/MGMTSocket.hpp
        src/Commands/AbstractCommand.cpp
        src/Commands/AbstractCommand.hpp
        src/Commands/GetMGMTInfoCommand.cpp
        src/Commands/GetMGMTInfoCommand.hpp
        src/Commands/CommandManager.cpp
        src/Commands/CommandManager.hpp
        src/Commands/CommandFactory.hpp)
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

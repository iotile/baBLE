cmake_minimum_required(VERSION 3.5)
project(baBLE)

################ VERSION #################
file (STRINGS "../../VERSION" VERSION)
add_definitions(-DVERSION="${VERSION}")
message("Version: ${VERSION}")

################ CONFIGURATION #################
set(CMAKE_CXX_STANDARD 14)

# To be sure that char = uint8_t
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -funsigned-char")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funsigned-char")

# To strip debug info on release
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# To link statically the libraries (libuv)
link_libraries("-static -static-libgcc")

############## CREATE EXECUTABLE ###############
## Build executable from sources files
set(SOURCES_FILES
        src/main.cpp
        src/utils/colors.hpp
        src/utils/stream_formats.hpp
        src/utils/string_formats.hpp
        src/Log/Log.cpp
        src/Log/Loggable.hpp
        src/Application/Packets/constants.hpp
        src/Application/Packets/PacketUuid.cpp
        src/Application/Packets/AbstractPacket.cpp
        src/Application/Packets/Base/HostToControllerPacket.cpp
        src/Application/Packets/Base/ControllerToHostPacket.cpp
        src/Application/Packets/Base/HostOnlyPacket.hpp
        src/Application/Packets/Commands/Disconnect/Disconnect.cpp
        src/Application/Packets/Commands/SetPowered/SetPoweredRequest.cpp
        src/Application/Packets/Commands/SetDiscoverable/SetDiscoverableRequest.cpp
        src/Application/Packets/Commands/SetConnectable/SetConnectableRequest.cpp
        src/Application/Packets/Commands/Read/ReadRequest.cpp
        src/Application/Packets/Commands/Write/WriteRequest.cpp
        src/Application/Packets/Commands/WriteWithoutResponse/WriteWithoutResponse.cpp
        src/Application/Packets/Commands/NotificationReceived/NotificationReceived.cpp
        src/Application/Packets/Commands/GetControllersIds/GetControllersIdsRequest.cpp
        src/Application/Packets/Commands/GetControllerInfo/GetControllerInfoRequest.cpp
        src/Application/Packets/Commands/GetConnectedDevices/GetConnectedDevicesRequest.cpp
        src/Application/Packets/Commands/ReadByType/ReadByTypeRequest.cpp
        src/Application/Packets/Commands/ReadByType/ReadByTypeResponse.cpp
        src/Application/Packets/Commands/ReadByGroupType/ReadByGroupTypeRequest.cpp
        src/Application/Packets/Commands/ReadByGroupType/ReadByGroupTypeResponse.cpp
        src/Application/Packets/Commands/GetConnectedDevices/GetConnectedDevicesResponse.cpp
        src/Application/Packets/Commands/GetControllerInfo/GetControllerInfoResponse.cpp
        src/Application/Packets/Commands/GetControllersIds/GetControllersIdsResponse.cpp
        src/Application/Packets/Commands/Read/ReadResponse.cpp
        src/Application/Packets/Commands/SetConnectable/SetConnectableResponse.cpp
        src/Application/Packets/Commands/SetDiscoverable/SetDiscoverableResponse.cpp
        src/Application/Packets/Commands/SetPowered/SetPoweredResponse.cpp
        src/Application/Packets/Commands/Write/WriteResponse.cpp
        src/Application/Packets/Commands/SetScanParameters/SetScanParameters.cpp
        src/Application/Packets/Commands/SetScanEnable/SetScanEnable.cpp
        src/Application/Packets/Commands/CreateConnection/CreateConnection.cpp
        src/Application/Packets/Commands/CancelConnection/CancelConnectionRequest.cpp
        src/Application/Packets/Commands/CancelConnection/CancelConnectionResponse.cpp
        src/Application/Packets/Events/CommandComplete/CommandComplete.cpp
        src/Application/Packets/Events/DeviceConnected/DeviceConnected.cpp
        src/Application/Packets/Events/DeviceDisconnected/DeviceDisconnected.cpp
        src/Application/Packets/Events/ControllerAdded/ControllerAdded.cpp
        src/Application/Packets/Events/ControllerRemoved/ControllerRemoved.cpp
        src/Application/Packets/Events/AdvertisingReport/AdvertisingReport.cpp
        src/Application/Packets/Events/CommandStatus/CommandStatus.cpp
        src/Application/Packets/Errors/BaBLEError/BaBLEError.cpp
        src/Application/Packets/Errors/ErrorResponse/ErrorResponse.cpp
        src/Application/Packets/Meta/GetControllersList/GetControllersList.cpp
        src/Application/Packets/Meta/ProbeServices/ProbeServices.cpp
        src/Application/Packets/Meta/ProbeCharacteristics/ProbeCharacteristics.cpp
        src/Application/Packets/Meta/StartScan/StartScan.cpp
        src/Application/Packets/Meta/StopScan/StopScan.cpp
        src/Application/Packets/Control/Ready/Ready.cpp
        src/Application/Packets/Control/Exit/Exit.cpp
        src/Application/PacketRouter/PacketRouter.cpp
        src/Application/PacketBuilder/PacketBuilder.cpp
        src/Format/AbstractFormat.hpp
        src/Format/AbstractExtractor.cpp
        src/Format/MGMT/constants.hpp
        src/Format/MGMT/MGMTFormat.hpp
        src/Format/MGMT/MGMTFormatBuilder.cpp
        src/Format/MGMT/MGMTFormatExtractor.cpp
        src/Format/Flatbuffers/FlatbuffersFormat.hpp
        src/Format/Flatbuffers/FlatbuffersFormatBuilder.cpp
        src/Format/Flatbuffers/FlatbuffersFormatExtractor.cpp
        src/Format/HCI/constants.hpp
        src/Format/HCI/HCIFormat.hpp
        src/Format/HCI/HCIFormatBuilder.cpp
        src/Format/HCI/HCIFormatExtractor.cpp
        src/Transport/AbstractSocket.hpp
        src/Transport/SocketContainer/SocketContainer.cpp
        src/Transport/Socket/MGMT/MGMTSocket.cpp
        src/Transport/Socket/StdIO/StdIOSocket.cpp
        src/Transport/Socket/HCI/HCISocket.cpp
        src/bootstrap.hpp
        src/Exceptions/BaBLEException.cpp)
add_executable(bable-bridge-linux ${SOURCES_FILES})

############# INCLUDE DEPENDENCIES #############
## libuv
include_directories(deps/uv/libuv/include)
add_subdirectory(deps/uv)
target_link_libraries(bable-bridge-linux uv)

## flatbuffers
add_subdirectory(deps/flatbuffers)
target_link_libraries(bable-bridge-linux flatbuffers)

## Generated flatbuffers protocol
include_directories(deps/protocol)

################# INSTALLATION #################
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/bable-bridge-linux DESTINATION bin)

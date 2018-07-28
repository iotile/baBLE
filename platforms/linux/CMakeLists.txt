cmake_minimum_required(VERSION 3.5)
project(baBLE)

option(BUILD_TESTS "Determines whether to build tests." OFF)

################ VERSION ##################
file (STRINGS "../../VERSION" VERSION)
add_definitions(-DVERSION="${VERSION}")
message("Version: ${VERSION}")

############## CONFIGURATION ###############
set(CMAKE_CXX_STANDARD 14)

# To be sure that char = uint8_t
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -funsigned-char")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funsigned-char")

# To strip debug info on release
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

############## CREATE LIBRARY ###############
## Build executable from sources files
set(SOURCES_FILES
        src/utils/colors.hpp
        src/utils/stream_formats.hpp
        src/utils/string_formats.hpp
        src/utils/string_formats.cpp
        src/utils/options_parser.hpp
        src/utils/options_parser.cpp
        src/Log/Log.cpp
        src/Log/Loggable.hpp
        src/Application/Packets/constants.hpp
        src/Application/Packets/PacketUuid.cpp
        src/Application/Packets/AbstractPacket.cpp
        src/Application/Packets/Base/HostToControllerPacket.cpp
        src/Application/Packets/Base/ControllerToHostPacket.cpp
        src/Application/Packets/Base/HostOnlyPacket.hpp
        src/Application/Packets/Base/ControllerOnlyPacket.hpp
        src/Application/Packets/Commands/Disconnect/Disconnect.cpp
        src/Application/Packets/Commands/SetPowered/SetPoweredRequest.cpp
        src/Application/Packets/Commands/SetPowered/SetPoweredResponse.cpp
        src/Application/Packets/Commands/Read/Central/ReadRequest.cpp
        src/Application/Packets/Commands/Read/Central/ReadResponse.cpp
        src/Application/Packets/Commands/Read/Peripheral/ReadRequest.cpp
        src/Application/Packets/Commands/Read/Peripheral/ReadResponse.cpp
        src/Application/Packets/Commands/WriteWithoutResponse/Central/WriteWithoutResponse.cpp
        src/Application/Packets/Commands/WriteWithoutResponse/Peripheral/WriteWithoutResponse.cpp
        src/Application/Packets/Commands/HandleValueNotification/NotificationReceived.cpp
        src/Application/Packets/Commands/HandleValueNotification/EmitNotification.cpp
        src/Application/Packets/Commands/GetControllersIds/GetControllersIdsRequest.cpp
        src/Application/Packets/Commands/GetControllerInfo/GetControllerInfoRequest.cpp
        src/Application/Packets/Commands/GetConnectedDevices/GetConnectedDevices.cpp
        src/Application/Packets/Commands/ReadByType/Peripheral/ReadByTypeRequest.cpp
        src/Application/Packets/Commands/ReadByType/Central/ReadByTypeRequest.cpp
        src/Application/Packets/Commands/ReadByType/Central/ReadByTypeResponse.cpp
        src/Application/Packets/Commands/ReadByGroupType/Central/ReadByGroupTypeRequest.cpp
        src/Application/Packets/Commands/ReadByGroupType/Central/ReadByGroupTypeResponse.cpp
        src/Application/Packets/Commands/ReadByGroupType/Peripheral/ReadByGroupType.cpp
        src/Application/Packets/Commands/GetControllerInfo/GetControllerInfoResponse.cpp
        src/Application/Packets/Commands/GetControllersIds/GetControllersIdsResponse.cpp
        src/Application/Packets/Commands/SetConnectable/SetConnectableRequest.cpp
        src/Application/Packets/Commands/SetConnectable/SetConnectableResponse.cpp
        src/Application/Packets/Commands/SetAdvertiseEnable/SetAdvertiseEnable.cpp
        src/Application/Packets/Commands/SetDiscoverable/SetDiscoverableRequest.cpp
        src/Application/Packets/Commands/SetDiscoverable/SetDiscoverableResponse.cpp
        src/Application/Packets/Commands/Write/Central/WriteRequest.cpp
        src/Application/Packets/Commands/Write/Central/WriteResponse.cpp
        src/Application/Packets/Commands/Write/Peripheral/WriteRequest.cpp
        src/Application/Packets/Commands/Write/Peripheral/WriteResponse.cpp
        src/Application/Packets/Commands/SetScanParameters/SetScanParameters.cpp
        src/Application/Packets/Commands/SetScanEnable/SetScanEnable.cpp
        src/Application/Packets/Commands/CreateConnection/CreateConnection.cpp
        src/Application/Packets/Commands/CancelConnection/CancelConnectionRequest.cpp
        src/Application/Packets/Commands/CancelConnection/CancelConnectionResponse.cpp
        src/Application/Packets/Commands/SetAdvertisingData/SetAdvertisingData.cpp
        src/Application/Packets/Commands/SetAdvertisingData/SetAdvertisingData.hpp
        src/Application/Packets/Commands/SetScanResponse/SetScanResponse.cpp
        src/Application/Packets/Commands/SetScanResponse/SetScanResponse.hpp
        src/Application/Packets/Commands/SetAdvertisingParameters/SetAdvertisingParameters.cpp
        src/Application/Packets/Commands/SetAdvertisingParameters/SetAdvertisingParameters.hpp
        src/Application/Packets/Commands/FindInformation/Peripheral/FindInformation.cpp
        src/Application/Packets/Commands/FindInformation/Peripheral/FindInformation.hpp
        src/Application/Packets/Commands/FindByType/Peripheral/FindByType.cpp
        src/Application/Packets/Events/CommandComplete/CommandComplete.cpp
        src/Application/Packets/Events/DeviceConnected/DeviceConnected.cpp
        src/Application/Packets/Events/DeviceDisconnected/DeviceDisconnected.cpp
        src/Application/Packets/Events/ControllerAdded/ControllerAdded.cpp
        src/Application/Packets/Events/ControllerRemoved/ControllerRemoved.cpp
        src/Application/Packets/Events/AdvertisingReport/AdvertisingReport.cpp
        src/Application/Packets/Events/CommandStatus/CommandStatus.cpp
        src/Application/Packets/Events/NumberOfCompletedPackets/NumberOfCompletedPackets.cpp
        src/Application/Packets/Errors/BaBLEError/BaBLEError.cpp
        src/Application/Packets/Errors/ErrorResponse/ErrorResponse.cpp
        src/Application/Packets/Meta/GetControllersList/GetControllersList.cpp
        src/Application/Packets/Meta/ProbeServices/ProbeServices.cpp
        src/Application/Packets/Meta/ProbeCharacteristics/ProbeCharacteristics.cpp
        src/Application/Packets/Meta/StartScan/StartScan.cpp
        src/Application/Packets/Meta/StopScan/StopScan.cpp
        src/Application/Packets/Meta/SetAdvertising/SetAdvertising.cpp
        src/Application/Packets/Meta/SetAdvertising/SetAdvertising.hpp
        src/Application/Packets/Control/SetGATTTable/SetGATTTable.cpp
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
        src/Transport/Socket/Socket.cpp
        src/Transport/Socket/MGMT/MGMTSocket.cpp
        src/Transport/Socket/StdIO/StdIOSocket.cpp
        src/Transport/Socket/HCI/HCISocket.cpp
        src/registration.hpp
        src/Exceptions/BaBLEException.cpp)
add_library(bable ${SOURCES_FILES})
target_include_directories(bable PUBLIC src)

#### Include dependencies ####
## libuv
add_subdirectory(deps/uv)
target_link_libraries(bable uv)

## flatbuffers
add_subdirectory(deps/flatbuffers)
target_link_libraries(bable flatbuffers)

## Generated flatbuffers protocol
target_include_directories(bable PUBLIC deps/protocol)

############## CREATE EXECUTABLE ###############
add_executable(bable-bridge-linux src/main.cpp)
target_link_libraries(bable-bridge-linux bable -static)  # Statically linked

########## CREATE TEST (IF ENABLED) ############
if (BUILD_TESTS)
    enable_testing()

    set(TEST_FILES
            tests/test_main.cpp
            tests/test_hci_format.cpp
            tests/test_mgmt_format.cpp
            tests/test_flatbuffers_format.cpp
            tests/test_string_formats.cpp
            tests/test_packet_uuid.cpp
            tests/test_integration.cpp
            tests/mocks/mock_socket.hpp
            tests/mocks/mock_stdio_socket.hpp
            tests/mocks/mock_hci_socket.hpp
            tests/mocks/mock_mgmt_socket.hpp)

    add_executable(bable-bridge-linux-test ${TEST_FILES})
    target_link_libraries(bable-bridge-linux-test bable)

    #### Include test dependencies ####
    ## Catch2 (for unit tests)
    target_include_directories(bable-bridge-linux-test PRIVATE deps/catch2)

    #### Declare test ####
    add_test(bable-bridge-linux-test bable-bridge-linux-test)
endif()

################# INSTALLATION #################
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/bable-bridge-linux DESTINATION bin)

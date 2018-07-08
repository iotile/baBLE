#ifndef BABLE_LINUX_PACKET_CONSTANTS_HPP
#define BABLE_LINUX_PACKET_CONSTANTS_HPP

namespace Packet {

  enum class Type {
    MGMT,
    HCI,
    FLATBUFFERS,
    NONE
  };

  enum class Id {
    None,
    BaBLEError,
    CancelConnectionRequest,
    CancelConnectionResponse,
    CommandComplete,
    CommandStatus,
    ControllerAdded,
    ControllerRemoved,
    CreateConnection,
    DeviceConnected,
    DeviceDisconnected,
    Disconnect,
    AdvertisingReport,
    NotificationReceived,
    ErrorResponse,
    Exit,
    GetConnectedDevices,
    GetControllerInfoRequest,
    GetControllerInfoResponse,
    GetControllersIdsRequest,
    GetControllersIdsResponse,
    GetControllersList,
    ProbeCharacteristics,
    ProbeServices,
    ReadRequest,
    ReadResponse,
    ReadByGroupTypeRequest,
    ReadByGroupTypeResponse,
    ReadByTypeRequest,
    ReadByTypeResponse,
    Ready,
    SetConnectableRequest,
    SetConnectableResponse,
    SetDiscoverableRequest,
    SetDiscoverableResponse,
    SetPoweredRequest,
    SetPoweredResponse,
    SetScanEnable,
    SetScanParameters,
    StartScan,
    StopScan,
    WriteRequest,
    WriteResponse,
    WriteWithoutResponse
  };

}

#endif //BABLE_LINUX_PACKET_CONSTANTS_HPP

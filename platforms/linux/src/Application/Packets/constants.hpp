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
    CommandComplete,
    ControllerAdded,
    ControllerRemoved,
    CreateConnection,
    DeviceConnected,
    DeviceDisconnected,
    Disconnect,
    Discovering,
    AdvertisingReport,
    NotificationReceived,
    ErrorResponse,
    Exit,
    GetConnectedDevicesRequest,
    GetConnectedDevicesResponse,
    GetControllerInfoRequest,
    GetControllerInfoResponse,
    GetControllersIdsRequest,
    GetControllersIdsResponse,
    GetControllersList,
    GetMGMTInfoRequest,
    GetMGMTInfoResponse,
    ProbeCharacteristics,
    ProbeServices,
    ReadRequest,
    ReadResponse,
    ReadByGroupTypeRequest,
    ReadByGroupTypeResponse,
    ReadByTypeRequest,
    ReadByTypeResponse,
    Ready,
    StartScan,
    SetConnectableRequest,
    SetConnectableResponse,
    SetDiscoverableRequest,
    SetDiscoverableResponse,
    SetPoweredRequest,
    SetPoweredResponse,
    SetScanEnable,
    SetScanParameters,
    WriteRequest,
    WriteResponse,
    WriteWithoutResponse
  };

}

#endif //BABLE_LINUX_PACKET_CONSTANTS_HPP

#ifndef BABLE_LINUX_PACKET_CONSTANTS_HPP
#define BABLE_LINUX_PACKET_CONSTANTS_HPP

namespace Packet {

  enum class Type {
    MGMT,
    HCI,
    ASCII,
    FLATBUFFERS,
    NONE
  };

  enum class Id {
    None,
    AddDeviceRequest,
    AddDeviceResponse,
    BaBLEError,
    ClassOfDeviceChanged,
    ControllerAdded,
    ControllerRemoved,
    DeviceAdded,
    DeviceConnected,
    DeviceDisconnected,
    DeviceFound,
    DeviceRemoved,
    DisconnectRequest,
    DisconnectResponse,
    Discovering,
    LEAdvertisingReport, // TODO: put with DeviceFound ???
    LEReadRemoteUsedFeaturesComplete,
    NewSettings,
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
    RemoveDeviceRequest,
    RemoveDeviceResponse,
    StartScanRequest,
    StartScanResponse,
    StopScanRequest,
    StopScanResponse,
    SetConnectableRequest,
    SetConnectableResponse,
    SetDiscoverableRequest,
    SetDiscoverableResponse,
    SetPoweredRequest,
    SetPoweredResponse,
    WriteRequest,
    WriteResponse,
    WriteWithoutResponse
  };

}

#endif //BABLE_LINUX_PACKET_CONSTANTS_HPP

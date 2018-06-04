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
    AddDeviceRequest,
    AddDeviceResponse,
    BaBLEError,
    ClassOfDeviceChanged,
    CommandComplete,
    ControllerAdded,
    ControllerRemoved,
    DeviceAdded,
    DeviceConnected,
    DeviceDisconnected,
    DeviceRemoved,
    DisconnectRequest,
    DisconnectResponse,
    Discovering,
    AdvertisingReport,
    ReadRemoteUsedFeaturesComplete,
    LocalNameChanged,
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
    StartScan,
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

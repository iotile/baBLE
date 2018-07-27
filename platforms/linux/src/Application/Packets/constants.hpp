#ifndef BABLE_PACKET_CONSTANTS_HPP
#define BABLE_PACKET_CONSTANTS_HPP

namespace Packet {

  enum class Type {
    MGMT,
    HCI,
    FLATBUFFERS,
    NONE
  };

  enum class Id {
    None,
    AdvertisingReport,
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
    EmitNotification,
    ErrorResponse,
    Exit,
    FindByType,
    FindInformation,
    GetConnectedDevices,
    GetControllerInfoRequest,
    GetControllerInfoResponse,
    GetControllersIdsRequest,
    GetControllersIdsResponse,
    GetControllersList,
    NotificationReceived,
    NumberOfCompletedPackets,
    ProbeCharacteristics,
    ProbeServices,
    ReadRequest,
    ReadResponse,
    ReadByGroupTypeRequest,
    ReadByGroupTypeResponse,
    ReadByTypeRequest,
    ReadByTypeResponse,
    Ready,
    SetAdvertising,
    SetAdvertisingData,
    SetAdvertiseEnable,
    SetAdvertisingParameters,
    SetConnectableRequest,
    SetConnectableResponse,
    SetDiscoverableRequest,
    SetDiscoverableResponse,
    SetGATTTable,
    SetPoweredRequest,
    SetPoweredResponse,
    SetScanEnable,
    SetScanParameters,
    SetScanResponse,
    StartScan,
    StopScan,
    WriteRequest,
    WriteResponse,
    WriteWithoutResponse
  };

}

#endif //BABLE_PACKET_CONSTANTS_HPP

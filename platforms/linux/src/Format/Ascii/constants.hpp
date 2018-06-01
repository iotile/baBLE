#ifndef BABLE_LINUX_ASCII_CONSTANTS_HPP
#define BABLE_LINUX_ASCII_CONSTANTS_HPP

namespace Format::Ascii {

  const char Delimiter = ',';

  enum CommandCode {
    ErrorResponse= 0x0000,

    GetMGMTInfo= 0x0001,
    StartScan= 0x0002,
    StopScan= 0x0003,
    AddDevice= 0x0004,
    RemoveDevice= 0x0005,
    Disconnect= 0x0006,
    SetPowered= 0x0007,
    SetDiscoverable= 0x0008,
    SetConnectable= 0x0009,
    GetControllersIds= 0x000A,
    GetControllerInfo= 0x000B,
    GetControllersList= 0x000C,
    GetConnectedDevices= 0x000D,

    Read= 0x000E,
    Write= 0x000F,
    WriteWithoutResponse= 0x00010,
    NotificationReceived= 0x0011,
    ReadByGroupTypeRequest= 0x0012,
    ReadByGroupTypeResponse= 0x0013,
    ProbeServices= 0x0014,
    ReadByTypeRequest= 0x0015,
    ReadByTypeResponse= 0x0016,
    ProbeCharacteristics= 0x0017,

    Exit= 0xFFFF
  };

  enum EventCode {
    DeviceFound= 0x0100,
    Discovering= 0x0200,
    DeviceConnected= 0x0300,
    DeviceDisconnected=0x0400,
    ClassOfDeviceChanged= 0x0500,
    NewSettings= 0x0600,
    ControllerAdded= 0x0700,
    ControllerRemoved= 0x0800,
    DeviceAdded= 0x0900,
    DeviceRemoved= 0x0A00,
    LEAdvertisingReport= 0x0B00,
    LEReadRemoteUsedFeaturesComplete= 0x0C00
  };

}

#endif //BABLE_LINUX_ASCII_CONSTANTS_HPP
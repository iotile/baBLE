#ifndef BABLE_LINUX_ASCII_CONSTANTS_HPP
#define BABLE_LINUX_ASCII_CONSTANTS_HPP

namespace Format::Ascii {

  const char Delimiter = ',';

  enum CommandCode {
    GetMGMTInfo= 0x0001,
    StartScan= 0x0002,
    StopScan= 0x0005,
    AddDevice= 0x0006,
    RemoveDevice= 0x0008,
    Disconnect= 0x0009,
    SetPowered= 0x000B
  };

  enum EventCode {
    DeviceFound= 0x0003,
    Discovering= 0x0004,
    DeviceConnected= 0x0007,
    DeviceDisconnected=0x000A,
    ClassOfDeviceChanged= 0x000C
  };

}

#endif //BABLE_LINUX_ASCII_CONSTANTS_HPP

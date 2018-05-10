#ifndef BABLE_LINUX_ASCII_CONSTANTS_HPP
#define BABLE_LINUX_ASCII_CONSTANTS_HPP

namespace Format::Ascii {

  const char Delimiter = ',';

  enum CommandCode {
    GetMGMTInfo= 0x0001,
    StartScan= 0x0002,
    StopScan= 0x005,
    AddDevice= 0x006
  };

  enum EventCode {
    DeviceFound= 0x003,
    Discovering= 0x004,
    DeviceConnected= 0x007
  };

}

#endif //BABLE_LINUX_ASCII_CONSTANTS_HPP

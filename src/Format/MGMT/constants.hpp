#ifndef BABLE_LINUX_MGMT_CONSTANTS_HPP
#define BABLE_LINUX_MGMT_CONSTANTS_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Format::MGMT {

  const size_t header_length = 6;
  const uint16_t non_controller_id = 0xFFFF;

  enum CommandCode {
    GetMGMTInfo= 0x0001,
    StartScan= 0x0023,
    StopScan= 0x0024
  };

  enum EventCode {
    DeviceFound= 0x0012,
    Discovering= 0x0013
  };

  enum StatusCode {
    Success= 0x00,
    UnknownCommand= 0x01,
    NotConnected= 0x02,
    Failed= 0x03,
    ConnectFailed= 0x04,
    AuthenticationFailed= 0x05,
    NotPaired= 0x06,
    NoResources= 0x07,
    Timeout= 0x08,
    AlreadyConnected= 0x09,
    Busy= 0x0A,
    Rejected= 0x0B,
    NotSupported= 0x0C,
    InvalidParameters= 0x0D,
    Disconnected= 0x0E,
    NotPowered= 0x0F,
    Cancelled= 0x10,
    InvalidIndex= 0x11,
    RFKilled= 0x12,
    AlreadyPaired= 0x13,
    PermissionDenied= 0x14
  };

  const std::unordered_map<uint8_t, std::string> StatusString = {
      { StatusCode::Success, "Success" },
      { StatusCode::UnknownCommand, "Unknown command" },
      { StatusCode::NotConnected, "Not connected" },
      { StatusCode::Failed, "Failed" },
      { StatusCode::ConnectFailed, "Connect failed" },
      { StatusCode::AuthenticationFailed, "Authentication failed" },
      { StatusCode::NotPaired, "Not paired" },
      { StatusCode::NoResources, "No resources" },
      { StatusCode::Timeout, "Timeout" },
      { StatusCode::AlreadyConnected, "Already connected" },
      { StatusCode::Busy, "Busy" },
      { StatusCode::Rejected, "Rejected" },
      { StatusCode::NotSupported, "Not supported" },
      { StatusCode::InvalidParameters, "Invalid parameters" },
      { StatusCode::Disconnected, "Disconnected" },
      { StatusCode::NotPowered, "Not powered" },
      { StatusCode::Cancelled, "Cancelled" },
      { StatusCode::InvalidIndex, "Invalid index" },
      { StatusCode::RFKilled, "RF killed" },
      { StatusCode::AlreadyPaired, "Already paired" },
      { StatusCode::PermissionDenied, "Permission denied" },
  };

}

#endif //BABLE_LINUX_MGMT_CONSTANTS_HPP

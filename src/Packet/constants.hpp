#ifndef BABLE_LINUX_MGMT_CODES_HPP
#define BABLE_LINUX_MGMT_CODES_HPP

#include <string>
#include <unordered_map>

namespace Packet {

  enum Type {
    MGMT,
    ASCII,
    FLATBUFFERS
  };

  namespace Commands {
    namespace MGMT {
      enum Code {
        GetMGMTInfo= 0x0001,
        StartScan= 0x0023,
        StopScan= 0x0024
      };
    }

    namespace Ascii {
      enum Code {
        GetMGMTInfo= 0x0001,
        StartScan= 0x0002,
        StopScan= 0x005
      };

      const char Delimiter = ',';
    }
  }

  namespace Events {
    namespace MGMT {
      enum Code {
        DeviceFound= 0x0012,
        Discovering= 0x0013
      };
    }

    namespace Ascii {
      enum Code {
        DeviceFound= 0x003,
        Discovering= 0x004
      };
    }
  }

  namespace Status {
    namespace MGMT {
      enum Code {
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

      const std::unordered_map<uint8_t, std::string> ToString = {
          { Code::Success, "Success" },
          { Code::UnknownCommand, "Unknown command" },
          { Code::NotConnected, "Not connected" },
          { Code::Failed, "Failed" },
          { Code::ConnectFailed, "Connect failed" },
          { Code::AuthenticationFailed, "Authentication failed" },
          { Code::NotPaired, "Not paired" },
          { Code::NoResources, "No resources" },
          { Code::Timeout, "Timeout" },
          { Code::AlreadyConnected, "Already connected" },
          { Code::Busy, "Busy" },
          { Code::Rejected, "Rejected" },
          { Code::NotSupported, "Not supported" },
          { Code::InvalidParameters, "Invalid parameters" },
          { Code::Disconnected, "Disconnected" },
          { Code::NotPowered, "Not powered" },
          { Code::Cancelled, "Cancelled" },
          { Code::InvalidIndex, "Invalid index" },
          { Code::RFKilled, "RF killed" },
          { Code::AlreadyPaired, "Already paired" },
          { Code::PermissionDenied, "Permission denied" },
      };
    }
  }

}

#endif //BABLE_LINUX_MGMT_CODES_HPP

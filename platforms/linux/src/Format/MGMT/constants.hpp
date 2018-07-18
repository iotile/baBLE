#ifndef BABLE_LINUX_MGMT_CONSTANTS_HPP
#define BABLE_LINUX_MGMT_CONSTANTS_HPP

namespace Format {

  namespace MGMT {

    const std::size_t header_length = 6;

    enum CommandCode {
      GetMGMTInfo = 0x0001,
      GetControllersList = 0x0003,
      GetControllerInfo = 0x0004,
      SetPowered = 0x0005,
      SetDiscoverable = 0x0006,
      SetConnectable = 0x0007,
      Disconnect = 0x0014,
      GetConnections = 0x0015,
      StartScan = 0x0023,
      StopScan = 0x0024,
      SetAdvertising = 0x0029,
      AddDevice = 0x0033,
      RemoveDevice = 0x0034
    };

    enum EventCode {
      CommandComplete = 0x0001,
      CommandStatus = 0x0002,
      IndexAdded = 0x0004,
      IndexRemoved = 0x0005,
      NewSettings = 0x0006,
      ClassOfDeviceChanged = 0x0007,
      LocalNameChanged = 0x0008,
      DeviceConnected = 0x000B,
      DeviceDisconnected = 0x000C,
      DeviceFound = 0x0012,
      Discovering = 0x0013,
      DeviceAdded = 0x001A,
      DeviceRemoved = 0x001B
    };

    enum StatusCode {
      Success = 0x00,
      UnknownCommand = 0x01,
      NotConnected = 0x02,
      Failed = 0x03,
      ConnectFailed = 0x04,
      AuthenticationFailed = 0x05,
      NotPaired = 0x06,
      NoResources = 0x07,
      Timeout = 0x08,
      AlreadyConnected = 0x09,
      Busy = 0x0A,
      Rejected = 0x0B,
      NotSupported = 0x0C,
      InvalidParameters = 0x0D,
      Disconnected = 0x0E,
      NotPowered = 0x0F,
      Cancelled = 0x10,
      InvalidIndex = 0x11,
      RFKilled = 0x12,
      AlreadyPaired = 0x13,
      PermissionDenied = 0x14
    };

    enum EIRType {
      Flags = 0x01,
      IncompleteUUID16ServiceClass = 0x02,
      UUID16ServiceClass = 0x03,
      IncompleteUUID32ServiceClass = 0x04,
      UUID32ServiceClass = 0x05,
      IncompleteUUID128ServiceClass = 0x06,
      UUID128ServiceClass = 0x07,
      CompleteDeviceName = 0x09,
      ManufacturerSpecific = 0xFF
    };

    const std::string Reasons[] = {
        "Unspecified",
        "Connection timeout",
        "Connection terminated by local host",
        "Connection terminated by remote host",
        "Connection terminated due to authentication failure"
    };

    // Structure representing the Extended Inquiry Response data
    struct EIR {
      uint8_t flags = 0;
      std::vector<uint8_t> uuid;
      uint16_t company_id = 0;
      std::vector<uint8_t> manufacturer_data;
      std::vector<uint8_t> device_name;
    };

    // Structure representing a Bluetooth controller
    struct Controller {
      uint16_t id = 0;
      std::array<uint8_t, 6> address;
      uint8_t bluetooth_version = 0;
      uint16_t manufacturer = 0;
      uint32_t supported_settings = 0;
      uint32_t current_settings = 0;
      std::array<uint8_t, 3> class_of_device{};
      std::string name;
      std::string short_name;
    };

  }

}

#endif //BABLE_LINUX_MGMT_CONSTANTS_HPP

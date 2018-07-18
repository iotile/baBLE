#ifndef BABLE_LINUX_CONSTANTS_HPP
#define BABLE_LINUX_CONSTANTS_HPP

#include "Transport/Socket/Socket.hpp"

// Protocol
#define BTPROTO_L2CAP   0
#define BTPROTO_HCI   1

// Channels
#define HCI_CHANNEL_RAW 0
#define HCI_CHANNEL_CONTROL 3

// For filter
#define SOL_HCI 0
#define HCI_FILTER 2

#define HCIGETDEVLIST _IOR('H', 210, int)
#define HCIGETDEVINFO _IOR('H', 211, int)
#define HCIGETCONNLIST _IOR('H', 212, int)
#define HCI_MAX_DEV 16
#define HCI_MAX_CONN 16

// BLE only supports ATT Channel ID for L2CAP
#define ATT_CID 4

// Misc.
#define NON_CONTROLLER_ID 0xFFFF

namespace Format {

  namespace HCI {

    const std::size_t command_header_length = 4;
    const std::size_t async_data_header_length = 10;
    const std::size_t event_header_length = 3;
    const std::size_t acl_mtu = 23;  // FIXME: mtu hardcoded (get from negociation using HCI)

    struct hci_filter {
      uint32_t type_mask;
      uint32_t event_mask1;
      uint32_t event_mask2;
      uint16_t opcode;
    };

    struct hci_dev_req {
      uint16_t dev_id;
      uint32_t dev_opt;
    };

    struct hci_dev_list_req {
      uint16_t dev_num;
      struct hci_dev_req dev_req[0];
    };

    struct hci_dev_info {
      uint16_t dev_id;
      char     name[8];

      bdaddr_t bdaddr;

      uint32_t flags;
      uint8_t  type;

      uint8_t  features[8];

      uint32_t pkt_type;
      uint32_t link_policy;
      uint32_t link_mode;

      uint16_t acl_mtu;
      uint16_t acl_pkts;
      uint16_t sco_mtu;
      uint16_t sco_pkts;

      // hci_dev_stats
      uint32_t err_rx;
      uint32_t err_tx;
      uint32_t cmd_tx;
      uint32_t evt_rx;
      uint32_t acl_tx;
      uint32_t acl_rx;
      uint32_t sco_tx;
      uint32_t sco_rx;
      uint32_t byte_rx;
      uint32_t byte_tx;
    };

    struct hci_conn_info {
      uint16_t handle;
      bdaddr_t bdaddr;
      uint8_t  type;
      uint8_t  out;
      uint16_t state;
      uint32_t link_mode;
    };

    struct hci_conn_list_req {
      uint16_t dev_id;
      uint16_t conn_num;
      struct hci_conn_info conn_info[0];
    };

    // Structure representing the Extended Inquiry Response data
    struct EIR {
      uint8_t flags = 0;
      std::vector<uint8_t> uuid;
      uint16_t company_id = 0;
      std::vector<uint8_t> manufacturer_data;
      std::vector<uint8_t> device_name;
    };

    enum AdvertisingReportType {
      ConnectableUndirected = 0x00,
      ConnectableDirected = 0x01,
      ScannableUndirected = 0x02,
      NonConnectableUndirected = 0x03,
      ScanResponse = 0x04
    };

    // Contains PacketBoundary Flag concatenated with Broadcast Flag=0b00 (No broadcast)
    enum HandleFlag {
      StartNonFlush = 0x00,
      Continuing = 0x01,
      StartFlush = 0x02,
      Complete= 0x03
    };

    enum Type {
      Command= 0x01,
      AsyncData= 0x02,
      SyncData= 0x03,
      Event= 0x04
    };

    enum ReportType {
      Flags= 0x01,
      IncompleteUUID16ServiceClass= 0x02,
      UUID16ServiceClass= 0x03,
      IncompleteUUID32ServiceClass= 0x04,
      UUID32ServiceClass= 0x05,
      IncompleteUUID128ServiceClass= 0x06,
      UUID128ServiceClass= 0x07,
      ShortDeviceName = 0x08,
      CompleteDeviceName= 0x09,
      UUID16ServiceData= 0x16,
      ManufacturerSpecific= 0xFF
    };

    enum EventCode {
      DisconnectComplete= 0x05,
      CommandComplete= 0x0E,
      CommandStatus= 0x0F,
      LEMeta= 0x3E
    };

    enum SubEventCode {
      LEConnectionComplete= (EventCode::LEMeta << 8) | 0x01,
      LEAdvertisingReport= (EventCode::LEMeta << 8) | 0x02,
      LEConnectionUpdateComplete= (EventCode::LEMeta << 8) | 0x03,
      LEReadRemoteUsedFeaturesComplete= (EventCode::LEMeta << 8) | 0x04
    };

    enum CommandCode {
      SetScanParameters = 0x200B,
      SetScanEnable = 0x200C,
      LECreateConnection = 0x200D,
      LECreateConnectionCancel = 0x200E,
      Disconnect = 0x0406,
      ConnectionParameterUpdateRequest = 0x0012
    };

    enum AttributeCode {
      ErrorResponse= 0x01,
      ReadByTypeRequest= 0x08,
      ReadByTypeResponse= 0x09,
      ReadRequest= 0x0A,
      ReadResponse= 0x0B,
      ReadByGroupTypeRequest= 0x10,
      ReadByGroupTypeResponse= 0x11,
      WriteRequest= 0x12,
      WriteResponse= 0x13,
      HandleValueNotification= 0x1B,
      WriteCommand= 0x52
    };

    enum StatusCode {
      Success= 0x00,
      UnknownHCICommand= 0x01,
      UnknownConnectionIdentifier= 0x02,
      HardwareFailure= 0x03,
      PageTimeout= 0x04,
      AuthenticationFailed= 0x05,
      PINOrKeyMissing= 0x06,
      MemoryCapacityExceeded= 0x07,
      ConnectionTimeout= 0x08,
      ConnectionLimitExceeded= 0x09,
      SynchronousConnectionLimitExceeded= 0x0A,
      ConnectionAlreadyExists= 0x0B,
      CommandDisallowed= 0x0C,
      ConnectionRejectedLimitedResources= 0x0D,
      ConnectionRejectedSecurityReasons= 0x0E,
      ConnectionRejectedUnacceptableBDADDR= 0x0F,
      ConnectionAcceptTimeout= 0x10,
      UnsupportedFeatureOrParameterValue= 0x11,
      InvalidHCICommandParameters= 0x12,
      RemoteUserTerminatedConnection= 0x13,
      RemoteDeviceTerminatedConnectionLowResources= 0x14,
      RemoteDeviceTerminatedConnectionPowerOff= 0x15,
      ConnectionTerminatedLocalHost= 0x16,
      RepeatedAttempts= 0x17,
      PairingNotAllowed= 0x18,
      UnknownLMPPDU= 0x19,
      UnsupportedRemoteFeature= 0x1A,
      SCOOffsetRejected= 0x1B,
      SCOIntervalRejected= 0x1C,
      SCOAirModeRejected= 0x1D,
      InvalidLMPParameters= 0x1E,
      UnspecifiedError= 0x1F,
      UnsupportedLMPParameterValue= 0x20,
      RoleChangeNotAllowed= 0x21,
      LMPResponseTimeout= 0x22,
      LMPErrorTransactionCollision= 0x23,
      LMPPDUNotAllowed= 0x24,
      EncryptionModeNotAcceptable= 0x25,
      LinkKeyCannotBeChanged= 0x26,
      RequestedQoSNotSupported= 0x27,
      InstantPassed= 0x28,
      PairingWithUnitKeyNotSupported= 0x29,
      DifferentTransactionCollision= 0x2A,
      QoSUnacceptableParameter= 0x2C,
      QoSRejected= 0x2D,
      ChannelClassificationNotSupported= 0x2E,
      InsufficientSecurity= 0x2F,
      ParameterOutOfMandatoryRange= 0x30,
      RoleSwitchPending= 0x32,
      ReservedSlotViolation= 0x34,
      RoleSwitchFailed= 0x35,
      ExtendedInquiryResponseTooLarge= 0x36,
      SecureSimplePairingNotSupportedHost= 0x37,
      HostBusyPairing= 0x38,
      ConnectionRejectedNoSuitableChannelFound= 0x39,
      ControllerBusy= 0x3A,
      UnacceptableConnectionParameters= 0x3B,
      AdvertisingTimeout= 0x3C,
      ConnectionTerminatedMICFailure= 0x3D,
      ConnectionFailedEstablished= 0x3E,
      MACConnectionFailed= 0x3F,
      CoarseClockAdjustmentRejected= 0x40,
      Type0SubmapNotDefined= 0x41,
      UnknownAdvertisingIdentifier= 0x42,
      LimitReached= 0x43,
      OperationCancelledHost= 0x44
    };

    enum AttributeErrorCode {
      None= 0x00,
      InvalidHandle= 0x01,
      ReadNotPermitted= 0x02,
      WriteNotPermitted= 0x03,
      InvalidPDU= 0x04,
      InsufficientAuthentication= 0x05,
      RequestNotSupported= 0x06,
      InvalidOffset= 0x07,
      InsufficientAuthorization= 0x08,
      PrepareQueueFull= 0x09,
      AttributeNotFound= 0x0A,
      AttributeNotLong= 0x0B,
      InsufficientEncryptionKeySize= 0x0C,
      InvalidAttributeValueLength= 0x0D,
      UnlikelyError= 0x0E,
      InsufficientEncryption= 0x0F,
      UnsupportedGroupType= 0x10,
      InsufficientResources= 0x11
    };

    // Structure representing a device
    struct Device {
      uint16_t connection_handle = 0;
      std::array<uint8_t, 6> address{};
    };

    // Structure representing a GATT service
    struct Service {
      uint16_t handle = 0;
      uint16_t group_end_handle = 0;
      std::vector<uint8_t> uuid{};
    };

    // Structure representing a GATT characteristic
    struct Characteristic {
      uint16_t handle = 0;
      uint8_t properties = 0;
      uint16_t value_handle = 0;
      uint16_t config_handle = 0;
      uint16_t configuration = 0;
      std::vector<uint8_t> uuid{};
    };

    enum GattUUID {
      PrimaryServiceDeclaration= 0x2800,
      CharacteristicDeclaration= 0x2803,
      ClientCharacteristicConfiguration= 0x2902
    };

  }

}

#endif //BABLE_LINUX_CONSTANTS_HPP

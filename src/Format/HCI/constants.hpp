#ifndef BABLE_LINUX_CONSTANTS_HPP
#define BABLE_LINUX_CONSTANTS_HPP

#include <cstdint>
#include <sys/socket.h>
#include <unordered_map>

// Protocol
#define BTPROTO_L2CAP   0
#define BTPROTO_HCI   1

// Channels
#define HCI_CHANNEL_RAW 0
#define HCI_CHANNEL_USER 1
#define HCI_CHANNEL_CONTROL 3

// For filter
#define SOL_HCI 0
#define HCI_FILTER 2

// BLE only supports ATT Channel ID for L2CAP
#define ATT_CID 4

// Misc.
#define NON_CONTROLLER_ID 0xFFFF

namespace Format::HCI {

  const std::size_t command_header_length = 4;
  const std::size_t async_data_header_length = 10;
  const std::size_t event_header_length = 3;

  struct sockaddr_hci {
    sa_family_t     hci_family;
    unsigned short  hci_dev;
    unsigned short  hci_channel;
  };

  typedef struct {
    uint8_t b[6];
  } __attribute__((packed)) bdaddr_t;

  struct sockaddr_l2 {
    sa_family_t l2_family;
    unsigned short l2_psm;
    bdaddr_t l2_bdaddr;
    unsigned short l2_cid;
    uint8_t l2_bdaddr_type;
  };

  struct hci_filter {
    uint32_t type_mask;
    uint32_t event_mask1;
    uint32_t event_mask2;
    uint16_t opcode;
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

  enum EventCode {
    DisconnectComplete= 0x05,
    LEMeta= 0x3e
  };

  enum SubEventCode {
    LEConnectionComplete= (EventCode::LEMeta << 8) | 0x01
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
    std::vector<uint8_t> uuid{};
  };

  enum UUID {
    GattPrimaryServiceDeclaration= 0x2800,
    GattCharacteristicDeclaration= 0x2803
  };

}

#endif //BABLE_LINUX_CONSTANTS_HPP

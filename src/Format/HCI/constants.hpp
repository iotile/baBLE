#ifndef BABLE_LINUX_CONSTANTS_HPP
#define BABLE_LINUX_CONSTANTS_HPP

#include <cstdint>
#include <sys/socket.h>

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
  const std::size_t async_data_header_length = 5;
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
    sa_family_t    l2_family;
    unsigned short l2_psm;
    bdaddr_t       l2_bdaddr;
    unsigned short l2_cid;
    uint8_t l2_bdaddr_type;
  };

  enum Type {
    Command= 0x01,
    AsyncData= 0x02,
    SyncData= 0x03,
    Event= 0x04
  };

  enum AttCode {
    ReadRequest= 0x0A,
    ReadResponse= 0x0B
  };

}

#endif //BABLE_LINUX_CONSTANTS_HPP

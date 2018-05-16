#ifndef BABLE_LINUX_CONSTANTS_HPP
#define BABLE_LINUX_CONSTANTS_HPP

#include <cstdint>
#include <sys/socket.h>

#define BTPROTO_HCI   1
#define HCI_CHANNEL_RAW 0
#define HCI_CHANNEL_USER 1
#define HCI_CHANNEL_CONTROL 3

namespace Format::HCI {

  const std::size_t command_header_length = 4;
  const std::size_t async_data_header_length = 5;
  const std::size_t event_header_length = 4;

  const uint16_t non_controller_id = 0xFFFF;

  struct sockaddr_hci {
    sa_family_t     hci_family;
    unsigned short  hci_dev;
    unsigned short  hci_channel;
  };

  enum Type {
    Command= 0x01,
    AsyncData= 0x02,
    SyncData= 0x03,
    Event= 0x04
  };

}

#endif //BABLE_LINUX_CONSTANTS_HPP

#ifndef BABLE_LINUX_PACKET_CONSTANTS_HPP
#define BABLE_LINUX_PACKET_CONSTANTS_HPP

namespace Packet {

  enum class Type {
    MGMT,
    HCI,
    ASCII,
    FLATBUFFERS,
    NONE
  };

}

#endif //BABLE_LINUX_PACKET_CONSTANTS_HPP

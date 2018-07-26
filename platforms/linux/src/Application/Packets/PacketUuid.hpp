#ifndef BABLE_PACKETUUID_HPP
#define BABLE_PACKETUUID_HPP

#include <cstdint>
#include "./constants.hpp"

namespace Packet {

  // Represents all the information to identify a response matching a waiting request
  class PacketUuid {

  public:
    PacketUuid(Packet::Type _packet_type, uint16_t _controller_id, uint16_t _connection_handle,
               uint16_t _response_packet_code, uint16_t _request_packet_code);

    PacketUuid(Packet::Type _packet_type, uint16_t _controller_id, uint16_t _connection_handle,
               uint16_t _response_packet_code);

    bool operator==(const PacketUuid& other) const;

    bool match(const PacketUuid& other) const;

    Packet::Type packet_type;
    uint16_t controller_id;
    uint16_t connection_handle;
    uint16_t response_packet_code;
    uint16_t request_packet_code;
  };

}

#endif //BABLE_PACKETUUID_HPPs

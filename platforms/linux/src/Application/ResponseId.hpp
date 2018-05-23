#ifndef BABLE_LINUX_RESPONSEID_HPP
#define BABLE_LINUX_RESPONSEID_HPP

#include <cstdint>
#include <string>
#include "Packets/constants.hpp"
#include "../Format/HCI/constants.hpp"

namespace Packet {

  // Represents all the information to identify a response matching a waiting request
  struct ResponseId {
    Packet::Type packet_type = Packet::Type::NONE;
    uint16_t controller_id = NON_CONTROLLER_ID;
    uint16_t connection_id = 0;
    uint16_t packet_code = 0;

    ResponseId(Packet::Type _packet_type, uint16_t _controller_id, uint16_t _connection_id, uint16_t _packet_code) {
      packet_type = _packet_type;
      controller_id = _controller_id;
      connection_id = _connection_id;
      packet_code = _packet_code;
    };

    ResponseId(Packet::Type _packet_type, uint16_t _controller_id, uint16_t _packet_code)
    : ResponseId(_packet_type, _controller_id, 0, _packet_code) {};

    bool operator==(const ResponseId& other) const {
      return packet_type == other.packet_type
          && controller_id == other.controller_id
          && connection_id == other.connection_id
          && packet_code == other.packet_code;
    };
  };

}

// To use ResponseId as a key in an unordered_map, we have to describe how to calculate a ResponseId hash (as global function)
namespace std {

  template <>
  struct hash<Packet::ResponseId>
  {
    std::size_t operator()(const Packet::ResponseId& response_id) const
    {
      return static_cast<size_t>(response_id.packet_type) << 48
          | static_cast<size_t>(response_id.controller_id) << 32
          | static_cast<size_t>(response_id.connection_id) << 16
          | static_cast<size_t>(response_id.packet_code);
    }
  };

}

#endif //BABLE_LINUX_RESPONSEID_HPP

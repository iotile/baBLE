#ifndef BABLE_LINUX_PacketUuid_HPP
#define BABLE_LINUX_PacketUuid_HPP

#include <cstdint>
#include <string>
#include "Packets/constants.hpp"
#include "../Format/HCI/constants.hpp"
#include "../Log/Log.hpp"

namespace Packet {

  // Represents all the information to identify a response matching a waiting request
  struct PacketUuid {
    Packet::Type packet_type = Packet::Type::NONE;
    uint16_t controller_id = NON_CONTROLLER_ID;
    uint16_t connection_id = 0;
    uint16_t response_packet_code = 0;
    uint16_t request_packet_code = 0;

    PacketUuid(Packet::Type _packet_type, uint16_t _controller_id, uint16_t _connection_id, uint16_t _response_packet_code, uint16_t _request_packet_code) {
      packet_type = _packet_type;
      controller_id = _controller_id;
      connection_id = _connection_id;
      response_packet_code = _response_packet_code;
      request_packet_code = _request_packet_code;
    };

    PacketUuid(Packet::Type _packet_type, uint16_t _controller_id, uint16_t _connection_id, uint16_t _response_packet_code)
      : PacketUuid(_packet_type, _controller_id, _connection_id, _response_packet_code, _response_packet_code){};

    bool operator==(const PacketUuid& other) const {
      if (packet_type != other.packet_type
          || controller_id != other.controller_id
          || response_packet_code != other.response_packet_code) {
        return false;
      }

      if (request_packet_code != response_packet_code && request_packet_code != other.request_packet_code) {
        return false;
      }

      if (connection_id != 0) {
        if (connection_id != other.connection_id) {
          return false;
        }
      }

      return true;
    };
  };

}

// To use PacketUuid as a key in an unordered_map, we have to describe how to calculate a PacketUuid hash (as global function)
namespace std {

  template <>
  struct hash<Packet::PacketUuid>
  {
    std::size_t operator()(const Packet::PacketUuid& response_id) const
    {
      return static_cast<size_t>(response_id.packet_type) << 48
          | static_cast<size_t>(response_id.controller_id) << 32
          | static_cast<size_t>(response_id.connection_id) << 16
          | static_cast<size_t>(response_id.response_packet_code);
    }
  };

}

#endif //BABLE_LINUX_PacketUuid_HPP

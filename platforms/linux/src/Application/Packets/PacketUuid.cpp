#include "PacketUuid.hpp"

namespace Packet {

  PacketUuid::PacketUuid(Packet::Type _packet_type, uint16_t _controller_id, uint16_t _connection_handle,
             uint16_t _response_packet_code, uint16_t _request_packet_code) {
    packet_type = _packet_type;
    controller_id = _controller_id;
    connection_handle = _connection_handle;
    response_packet_code = _response_packet_code;
    request_packet_code = _request_packet_code;
  }

  PacketUuid::PacketUuid(Packet::Type _packet_type, uint16_t _controller_id, uint16_t _connection_handle,
             uint16_t _response_packet_code)
      : PacketUuid(_packet_type, _controller_id, _connection_handle, _response_packet_code, _response_packet_code){};

  bool PacketUuid::operator==(const PacketUuid& other) const {
    return packet_type == other.packet_type
        && controller_id == other.controller_id
        && connection_handle == other.connection_handle
        && response_packet_code == other.response_packet_code
        && request_packet_code == other.request_packet_code;
  }

  bool PacketUuid::match(const PacketUuid& other) const {
    if (packet_type != other.packet_type
        || controller_id != other.controller_id
        || response_packet_code != other.response_packet_code) {
      return false;
    }

    if (request_packet_code != response_packet_code && request_packet_code != other.request_packet_code) {
      return false;
    }

    if (other.connection_handle != 0) {
      if (connection_handle != other.connection_handle) {
        return false;
      }
    }

    return true;
  }

}
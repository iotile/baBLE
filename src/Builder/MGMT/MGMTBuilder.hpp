#ifndef BABLE_LINUX_MGMTBUILDER_HPP
#define BABLE_LINUX_MGMTBUILDER_HPP

#include <memory>
#include "../AbstractBuilder.hpp"
#include "../../Serializer/Deserializer.hpp"
#include "../../Packet/AbstractPacket.hpp"
#include "../../Packet/constants.hpp"

class MGMTBuilder : public AbstractBuilder {

public:
  std::unique_ptr<Packet::AbstractPacket> build(Deserializer& deser) override {
    Deserializer tmp_deser = deser;

    uint16_t event_code;
    tmp_deser >> event_code;

    if (event_code == 0x0001 || event_code == 0x0002) {
      return build_command(deser);
    } else if (0x0002 < event_code && event_code <= 0x0025) {
      return build_event(event_code, deser);
    } else {
      throw std::invalid_argument("Unknown event code (event_code=" + std::to_string(event_code) + ")");
    }
  };

  std::unique_ptr<Packet::AbstractPacket> build_command(Deserializer& deser) {
    Deserializer tmp_deser = deser;

    // Extract event_code from deser
    uint16_t event_code;
    uint16_t controller_index;
    uint16_t payload_length;
    uint16_t command_code;

    tmp_deser >> event_code >> controller_index >> payload_length >> command_code;

    auto it = m_commands.find(command_code);
    if (it == m_commands.end()) {
      throw std::invalid_argument("Command code not found");
    }

    PacketConstructor fn = it->second;
    std::unique_ptr<Packet::AbstractPacket> packet = fn();
    packet->unserialize(deser);
    return packet;
  }

  std::unique_ptr<Packet::AbstractPacket> build_event(uint16_t event_code, Deserializer& deser) {
    auto it = m_events.find(event_code);
    if (it == m_events.end()) {
      throw std::invalid_argument("Event code not found");
    }

    // Get command from command_code
    PacketConstructor fn = it->second;
    std::unique_ptr<Packet::AbstractPacket> packet = fn();
    packet->unserialize(deser);
    return packet;
  }

private:
  const Packet::Type packet_type() const override {
    return Packet::Type::MGMT;
  };

};

#endif //BABLE_LINUX_MGMTBUILDER_HPP

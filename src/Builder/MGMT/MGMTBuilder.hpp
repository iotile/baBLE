#ifndef BABLE_LINUX_MGMTBUILDER_HPP
#define BABLE_LINUX_MGMTBUILDER_HPP

#include <memory>
#include "../AbstractBuilder.hpp"
#include "../../Serializer/Deserializer.hpp"
#include "../../Packet/AbstractPacket.hpp"
#include "../../Packet/constants.hpp"

class MGMTBuilder : public AbstractBuilder {

public:
  uint16_t extract_command_code(const Deserializer& deser) {
    Deserializer tmp_deser = deser;

    // Extract event_code from deser
    uint16_t event_code;
    uint16_t controller_index;
    uint16_t payload_length;
    tmp_deser >> event_code >> controller_index >> payload_length;

    // If command event, extract command_code from deser
    if (event_code == 0x0001 || event_code == 0x0002) {
      uint16_t command_code;
      tmp_deser >> command_code;
      return command_code;
    }

    throw std::invalid_argument("Can't find command_code in given data. Perhaps it is not a command packet... (event_code=" + std::to_string(event_code) + ")");
  }

  std::unique_ptr<Packet::AbstractPacket> build(Deserializer& deser) override {
    uint16_t command_code = extract_command_code(deser);

    auto it = m_packets.find(command_code);
    if (it == m_packets.end()) {
      throw std::invalid_argument("Command code not found");
    }

    // Get command from command_code
    PacketConstructor fn = it->second;
    std::unique_ptr<Packet::AbstractPacket> packet = fn();
    packet->unserialize(deser);
    return packet;
  };

private:
  const Packet::Type packet_type() const override {
    return Packet::Type::MGMT;
  };

};

#endif //BABLE_LINUX_MGMTBUILDER_HPP

#ifndef BABLE_LINUX_ASCIIBUILDER_HPP
#define BABLE_LINUX_ASCIIBUILDER_HPP

#include <memory>
#include "../AbstractBuilder.hpp"
#include "../../Serializer/Deserializer.hpp"
#include "../../Packet/AbstractPacket.hpp"
#include "../../Packet/constants.hpp"

class AsciiBuilder : public AbstractBuilder {

public:
  std::unique_ptr<Packet::AbstractPacket> build(Deserializer& deser) override {
    // Extract event_code from deser
    std::string command_code_str;
    deser >> command_code_str;
    auto command_code = static_cast<uint16_t>(stoi(command_code_str));

    auto it = m_commands.find(command_code);
    if (it == m_commands.end()) {
      throw std::invalid_argument("Command code not found");
    }

    // Get command from command_code
    PacketConstructor fn = it->second;
    std::unique_ptr<Packet::AbstractPacket> packet = fn();
    packet->unserialize(deser);
    return packet;
  };

  template<class T>
  AbstractBuilder& register_event(Packet::Type translated_type) = delete;

private:
  const Packet::Type packet_type() const override {
      return Packet::Type::ASCII;
  };

};

#endif //BABLE_LINUX_ASCIIBUILDER_HPP

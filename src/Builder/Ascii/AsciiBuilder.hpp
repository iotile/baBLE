#ifndef BABLE_LINUX_ASCIIBUILDER_HPP
#define BABLE_LINUX_ASCIIBUILDER_HPP

#include <memory>
#include "../AbstractBuilder.hpp"
#include "../../Serializer/Deserializer.hpp"
#include "../../Packet/AbstractPacket.hpp"
#include "../../Packet/constants.hpp"

class AsciiBuilder : public AbstractBuilder {

public:
  using AbstractBuilder::AbstractBuilder;

  static std::string extract_command_code(const Deserializer& deser) {
    Deserializer tmp_deser = deser;
    std::string data_str;
    tmp_deser >> data_str;

    size_t pos = data_str.find(Packet::Commands::Ascii::Delimiter);

    if (pos == std::string::npos) {
      return data_str;
    }

    std::string command_code_str = data_str.substr(0, pos);
    return command_code_str;
  }

  std::unique_ptr<Packet::AbstractPacket> build(Deserializer& deser) override {
    std::string command_code_str = extract_command_code(deser);

    auto command_code = static_cast<uint16_t>(stoi(command_code_str));

    auto it = m_commands.find(command_code);
    if (it == m_commands.end()) {
      throw std::invalid_argument("Command code not found");
    }

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

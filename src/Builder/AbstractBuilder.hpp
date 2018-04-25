#ifndef BABLE_LINUX_ABSTRACTBUILDER_HPP
#define BABLE_LINUX_ABSTRACTBUILDER_HPP

#include <functional>
#include <unordered_map>
#include "../Packet/AbstractPacket.hpp"
#include "../Serializer/Deserializer.hpp"

using PacketConstructor = std::function<std::unique_ptr<Packet::AbstractPacket>()>;

class AbstractBuilder {

public:
  template<class T>
  AbstractBuilder& register_packet(Packet::Type translated_type) {
    const Packet::Type initial_type = packet_type();
    uint16_t command_code = T::command_code(initial_type);

    auto it = m_packets.find(command_code);
    if (it != m_packets.end()) {
      throw std::invalid_argument("Packet already registered");
    }

    m_packets[command_code] = [initial_type, translated_type](){
      return std::make_unique<T>(initial_type, translated_type);
    };

    return *this;
  };

  virtual std::unique_ptr<Packet::AbstractPacket> build(Deserializer& deser) = 0;

protected:
  virtual const Packet::Type packet_type() const = 0;

  std::unordered_map<uint16_t, PacketConstructor> m_packets;

};

#endif //BABLE_LINUX_ABSTRACTBUILDER_HPP

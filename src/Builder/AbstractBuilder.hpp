#ifndef BABLE_LINUX_ABSTRACTBUILDER_HPP
#define BABLE_LINUX_ABSTRACTBUILDER_HPP

#include <functional>
#include <unordered_map>
#include "../Packet/AbstractPacket.hpp"
#include "../Serializer/Deserializer.hpp"

using PacketConstructor = std::function<std::unique_ptr<Packet::AbstractPacket>()>;

class AbstractBuilder {

public:
  AbstractBuilder() = default;

  explicit AbstractBuilder(Packet::Type default_translated_type) {
    m_default_translated_type = default_translated_type;
  }

  template<class T>
  AbstractBuilder& register_command() {
    if (m_default_translated_type == 0) {
      throw std::invalid_argument("Missing argument 'translated_type' to register command.");
    }

    return register_command<T>(m_default_translated_type);
  };

  template<class T>
  AbstractBuilder& register_command(Packet::Type translated_type) {
    const Packet::Type initial_type = packet_type();
    uint16_t command_code = T::command_code(initial_type);

    auto it = m_commands.find(command_code);
    if (it != m_commands.end()) {
      throw std::invalid_argument("Packet already registered");
    }

    m_commands[command_code] = [initial_type, translated_type](){
      return std::make_unique<T>(initial_type, translated_type);
    };

    return *this;
  };

  template<class T>
  AbstractBuilder& register_event() {
    if (m_default_translated_type == 0) {
      throw std::invalid_argument("Missing argument 'translated_type' to register event.");
    }

    return register_event<T>(m_default_translated_type);
  };

  template<class T>
  AbstractBuilder& register_event(Packet::Type translated_type) {
    const Packet::Type initial_type = packet_type();
    uint16_t event_code = T::event_code(initial_type);

    auto it = m_events.find(event_code);
    if (it != m_events.end()) {
      throw std::invalid_argument("Packet already registered");
    }

    m_events[event_code] = [initial_type, translated_type](){
      return std::make_unique<T>(initial_type, translated_type);
    };

    return *this;
  };

  virtual std::unique_ptr<Packet::AbstractPacket> build(Deserializer& deser) = 0;

protected:
  virtual const Packet::Type packet_type() const = 0;

  std::unordered_map<uint16_t, PacketConstructor> m_commands;
  std::unordered_map<uint16_t, PacketConstructor> m_events;

  Packet::Type m_default_translated_type;

};

#endif //BABLE_LINUX_ABSTRACTBUILDER_HPP

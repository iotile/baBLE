#ifndef BABLE_LINUX_PACKETBUILDER_HPP
#define BABLE_LINUX_PACKETBUILDER_HPP

#include "../AbstractPacket.hpp"
#include "../../Format/AbstractFormat.hpp"

// Used to register all the packets on startup and then build them from received raw bytes.
class PacketBuilder : public Loggable {

  // Define packet constructor function prototype
  using PacketConstructor = std::function<std::shared_ptr<Packet::AbstractPacket>()>;

public:
  // Constructors
  explicit PacketBuilder(std::shared_ptr<AbstractFormat> building_format);

  // Setters
  PacketBuilder& set_output_format(std::shared_ptr<AbstractFormat> output_format);

  // To register packets
  template<class T>
  PacketBuilder& register_command();
  template<class T>
  PacketBuilder& register_event();

  // To build packets
  std::shared_ptr<Packet::AbstractPacket> build(std::shared_ptr<AbstractExtractor>);
  std::shared_ptr<Packet::AbstractPacket> build_command(std::shared_ptr<AbstractExtractor>);
  std::shared_ptr<Packet::AbstractPacket> build_event(std::shared_ptr<AbstractExtractor>);

  const std::string stringify() const override;
  const std::shared_ptr<AbstractFormat> get_building_format() const {
    return m_building_format;
  };

private:
  std::shared_ptr<AbstractFormat> m_building_format; // Format used to import data after building packet
  std::shared_ptr<AbstractFormat> m_output_format; // Format used after packet translation

  // Commands and events are splitted to prevent command_code == event_code issues
  std::unordered_map<uint16_t, PacketConstructor> m_commands{};
  std::unordered_map<uint16_t, PacketConstructor> m_events{};

};

template<class T>
PacketBuilder& PacketBuilder::register_command() {
  const Packet::Type initial_type = m_building_format->get_packet_type();
  uint16_t command_code = T::packet_code(initial_type);

  auto it = m_commands.find(command_code);
  if (it != m_commands.end()) {
    throw std::invalid_argument("Packet already registered");
  }

  Packet::Type translated_type;

  if (m_output_format == nullptr) {
    translated_type = Packet::Type::NONE;
  } else {
    translated_type = m_output_format->get_packet_type();
  }

  m_commands.emplace(command_code, [initial_type, translated_type](){
    return std::make_shared<T>(initial_type, translated_type);
  });

  return *this;
};

template<class T>
PacketBuilder& PacketBuilder::register_event() {
  const Packet::Type initial_type = m_building_format->get_packet_type();
  uint16_t event_code = T::packet_code(initial_type);

  auto it = m_events.find(event_code);
  if (it != m_events.end()) {
    throw std::invalid_argument("Packet already registered");
  }

  Packet::Type translated_type;

  if (m_output_format == nullptr) {
    translated_type = Packet::Type::NONE;
  } else {
    translated_type = m_output_format->get_packet_type();
  }

  m_events.emplace(event_code, [initial_type, translated_type](){
    return std::make_shared<T>(initial_type, translated_type);
  });

  return *this;
};

#endif //BABLE_LINUX_PACKETBUILDER_HPP

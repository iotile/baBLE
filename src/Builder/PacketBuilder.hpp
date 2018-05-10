#ifndef BABLE_LINUX_PACKETBUILDER_HPP
#define BABLE_LINUX_PACKETBUILDER_HPP

#include <functional>
#include <memory>
#include <iostream>
#include <unordered_map>
#include "../Packet/AbstractPacket.hpp"
#include "../Format/AbstractFormat.hpp"
#include "../Exceptions/NotFound/NotFoundException.hpp"

// Used to register all the packets on startup and then build them from received raw bytes.
class PacketBuilder {

  // Define packet constructor function prototype
  using PacketConstructor = std::function<std::unique_ptr<Packet::AbstractPacket>()>;

public:
  // Constructors
  explicit PacketBuilder(std::shared_ptr<AbstractFormat> building_format);
  PacketBuilder(std::shared_ptr<AbstractFormat> building_format, std::shared_ptr<AbstractFormat> output_format) ;

  // Setters
  PacketBuilder& set_output_format(std::shared_ptr<AbstractFormat> output_format);

  // To register packets
  template<class T>
  PacketBuilder& register_command();
  template<class T>
  PacketBuilder& register_event();

  // To build packets
  std::unique_ptr<Packet::AbstractPacket> build(const std::vector<uint8_t>& raw_data);
  std::unique_ptr<Packet::AbstractPacket> build_command(const std::vector<uint8_t>& raw_data);
  std::unique_ptr<Packet::AbstractPacket> build_event(uint16_t event_code, const std::vector<uint8_t>& raw_data);

private:
  std::shared_ptr<AbstractFormat> m_building_format; // Format used to import data after building packet
  std::shared_ptr<AbstractFormat> m_output_format; // Format used after packet translation

  // Commands and events are splitted to prevent command_code == event_code issues
  std::unordered_map<uint16_t, PacketConstructor> m_commands{};
  std::unordered_map<uint16_t, PacketConstructor> m_events{};

};

template<class T>
PacketBuilder& PacketBuilder::register_command() {
  if (m_output_format == nullptr) {
    throw std::runtime_error("No output format specified in PacketBuilder. Can't register command.");
  }

  const Packet::Type initial_type = m_building_format->packet_type();
  uint16_t command_code = T::command_code(initial_type);

  auto it = m_commands.find(command_code);
  if (it != m_commands.end()) {
    throw std::invalid_argument("Packet already registered");
  }

  const Packet::Type translated_type = m_output_format->packet_type();

  m_commands.emplace(command_code, [initial_type, translated_type](){
    return std::make_unique<T>(initial_type, translated_type);
  });

  return *this;
};

template<class T>
PacketBuilder& PacketBuilder::register_event() {
  if (m_output_format == nullptr) {
    throw std::runtime_error("No output format specified in PacketBuilder. Can't register event.");
  }

  const Packet::Type initial_type = m_building_format->packet_type();
  uint16_t event_code = T::event_code(initial_type);

  auto it = m_events.find(event_code);
  if (it != m_events.end()) {
    throw std::invalid_argument("Packet already registered");
  }

  const Packet::Type translated_type = m_output_format->packet_type();

  m_events.emplace(event_code, [initial_type, translated_type](){
    return std::make_unique<T>(initial_type, translated_type);
  });

  return *this;
};

#endif //BABLE_LINUX_PACKETBUILDER_HPP

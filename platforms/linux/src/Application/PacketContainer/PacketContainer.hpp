#ifndef BABLE_LINUX_PACKETBUILDER_HPP
#define BABLE_LINUX_PACKETBUILDER_HPP

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <iostream>
#include <unordered_map>
#include "../AbstractPacket.hpp"
#include "../../Format/AbstractFormat.hpp"
#include "../../Exceptions/NotFound/NotFoundException.hpp"

// Used to register all the packets on startup and then build them from received raw bytes.
class PacketContainer : public Loggable {

  // Define packet constructor function prototype
  using PacketConstructor = std::function<std::shared_ptr<Packet::AbstractPacket>()>;
  using TimePoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>;

public:
  static void register_response(std::shared_ptr<Packet::AbstractPacket> packet);
  static void expire_waiting_packets(int64_t expiration_duration);

  // Constructors
  explicit PacketContainer(std::shared_ptr<AbstractFormat> building_format);

  // Setters
  PacketContainer& set_output_format(std::shared_ptr<AbstractFormat> output_format);

  // To register packets
  template<class T>
  PacketContainer& register_command();
  template<class T>
  PacketContainer& register_event();

  // To build packets
  std::shared_ptr<Packet::AbstractPacket> build(std::shared_ptr<AbstractExtractor>);
  std::shared_ptr<Packet::AbstractPacket> build_command(std::shared_ptr<AbstractExtractor>);
  std::shared_ptr<Packet::AbstractPacket> build_event(std::shared_ptr<AbstractExtractor>);

  const std::string stringify() const override;

private:
  std::shared_ptr<AbstractFormat> m_building_format; // Format used to import data after building packet
  std::shared_ptr<AbstractFormat> m_output_format; // Format used after packet translation

  // Commands and events are splitted to prevent command_code == event_code issues
  std::unordered_map<uint16_t, PacketConstructor> m_commands{};
  std::unordered_map<uint16_t, PacketConstructor> m_events{};

  static std::unordered_map<Packet::ResponseId, std::shared_ptr<Packet::AbstractPacket>> m_waiting_packets;
  static std::multimap<TimePoint, Packet::ResponseId> m_expiration_waiting_packets;

};

template<class T>
PacketContainer& PacketContainer::register_command() {
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
PacketContainer& PacketContainer::register_event() {
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

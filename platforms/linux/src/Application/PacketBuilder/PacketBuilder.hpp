#ifndef BABLE_PACKETBUILDER_HPP
#define BABLE_PACKETBUILDER_HPP

#include "Application/Packets/AbstractPacket.hpp"
#include "Format/AbstractFormat.hpp"
#include <unordered_map>
#include <unordered_set>

// Used to register all the packets on startup and then build them from received raw bytes.
class PacketBuilder : public Loggable {

  // Define packet constructor function prototype
  using PacketConstructor = std::function<std::shared_ptr<Packet::AbstractPacket>()>;

public:
  // Constructors
  explicit PacketBuilder(std::shared_ptr<AbstractFormat> building_format);

  // Setters
  PacketBuilder& set_ignored_packets(std::unordered_set<uint16_t> ignored_packets);

  // To register packets
  template<class T>
  PacketBuilder& register_command();
  template<class T>
  PacketBuilder& register_event();

  // To build packets
  std::shared_ptr<Packet::AbstractPacket> build(std::shared_ptr<AbstractExtractor>);

  const std::string stringify() const override;

private:
  std::shared_ptr<Packet::AbstractPacket> build_packet(std::shared_ptr<AbstractExtractor>extractor,
                                                       const std::unordered_map<uint16_t, PacketConstructor>& packets);
  template<class T>
  PacketBuilder& register_packet(std::unordered_map<uint16_t, PacketConstructor>& packets);

  std::shared_ptr<AbstractFormat> m_building_format; // Format used to import data after building packet

  // Commands and events are splitted to prevent command_code == event_code issues
  std::unordered_map<uint16_t, PacketConstructor> m_commands{};
  std::unordered_map<uint16_t, PacketConstructor> m_events{};

  std::unordered_set<uint16_t> m_ignored_packets{};

};

template<class T>
PacketBuilder& PacketBuilder::register_command() {
  return register_packet<T>(m_commands);
};

template<class T>
PacketBuilder& PacketBuilder::register_event() {
  return register_packet<T>(m_events);
};

template<class T>
PacketBuilder& PacketBuilder::register_packet(std::unordered_map<uint16_t, PacketBuilder::PacketConstructor>& packets) {
  Packet::Type format_type = m_building_format->get_packet_type();
  Packet::Type packet_type = T::initial_type();
  uint16_t packet_code = T::initial_packet_code();

  if (format_type != packet_type) {
    throw std::invalid_argument("Can't register given packet in this PacketBuilder: types not matching ("
                                    + std::to_string(packet_code) + ")");
  }

  auto it = packets.find(packet_code);
  if (it != packets.end()) {
    throw std::invalid_argument("Packet already registered");
  }

  packets.emplace(packet_code, [](){
    return std::make_shared<T>();
  });

  return *this;
};

#endif //BABLE_PACKETBUILDER_HPP

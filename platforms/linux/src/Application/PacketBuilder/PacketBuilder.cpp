#include "PacketBuilder.hpp"
#include "../../Log/Log.hpp"
#include "../../Exceptions/NotFound/NotFoundException.hpp"

using namespace std;

// Constructors
PacketBuilder::PacketBuilder(shared_ptr<AbstractFormat> building_format) {
  m_building_format = move(building_format);
  m_output_format = nullptr;
}

// Setters
PacketBuilder& PacketBuilder::set_output_format(shared_ptr<AbstractFormat> output_format) {
  m_output_format = move(output_format);
  return *this;
}

PacketBuilder& PacketBuilder::set_ignored_packets(std::unordered_set<uint16_t> ignored_packets) {
  m_ignored_packets = move(ignored_packets);

  return *this;
}

// To build packets
shared_ptr<Packet::AbstractPacket> PacketBuilder::build(std::shared_ptr<AbstractExtractor> extractor) {
  uint16_t type_code = extractor->get_type_code();

  if (m_building_format->is_command(type_code)) {
    return build_command(std::move(extractor));
  } else if (m_building_format->is_event(type_code)) {
    return build_event(std::move(extractor));
  } else {
    throw Exceptions::NotFoundException("Given data to build a packet has no known type: " + to_string(type_code));
  }
}

shared_ptr<Packet::AbstractPacket> PacketBuilder::build_command(std::shared_ptr<AbstractExtractor> extractor) {
  // Extract packet_code from data
  uint16_t command_code = extractor->get_packet_code();

  // Get command from packet_code
  auto command_it = m_commands.find(command_code);
  if (command_it == m_commands.end()) {
    if (m_ignored_packets.find(command_code) == m_ignored_packets.end()) {
      LOG.critical("Event code not found in PacketBuilder registry: " + to_string(command_code), "PacketBuilder");
      LOG.debug(extractor->get_raw_data(), "PacketBuilder");
    } else {
      LOG.debug("Command packet, in ignored list, has been ignored (" + to_string(command_code) + ")");
    }

    return nullptr;
  }

  PacketConstructor fn = command_it->second;
  shared_ptr<Packet::AbstractPacket> packet = fn();
  packet->from_bytes(extractor);
  return packet;
}

shared_ptr<Packet::AbstractPacket> PacketBuilder::build_event(std::shared_ptr<AbstractExtractor> extractor) {
  uint16_t event_code = extractor->get_packet_code();

  // Get event from event_code
  auto event_it = m_events.find(event_code);
  if (event_it == m_events.end()) {
    if (m_ignored_packets.find(event_code) == m_ignored_packets.end()) {
      LOG.critical("Event code not found in PacketBuilder registry: " + to_string(event_code), "PacketBuilder");
      LOG.debug(extractor->get_raw_data(), "PacketBuilder");
    } else {
      LOG.debug("Event packet, in ignored list, has been ignored (" + to_string(event_code) + ")");
    }

    return nullptr;
  }

  PacketConstructor fn = event_it->second;
  shared_ptr<Packet::AbstractPacket> packet = fn();
  packet->from_bytes(extractor);
  return packet;
}

const std::string PacketBuilder::stringify() const {
  std::stringstream result;
  result << "Commands: " << std::endl;

  for (auto& element : m_commands) {
    result << "\t" << element.first << std::endl;
  }

  result << std::endl << "Events: " << std::endl;
  for (auto& element : m_events) {
    result << "\t" << element.first << std::endl;
  }

  return result.str();
}

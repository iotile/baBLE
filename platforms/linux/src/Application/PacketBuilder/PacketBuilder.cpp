#include "PacketBuilder.hpp"

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
  Packet::Type packet_type = m_building_format->get_packet_type();
  uint16_t command_code = extractor->get_packet_code();

  // Get command from packet_code
  auto command_it = m_commands.find(command_code);
  if (command_it == m_commands.end()) {
    LOG.debug(extractor->get_raw_data(), "PacketBuilder");
    throw Exceptions::NotFoundException("Command code not found in PacketBuilder registry: " + to_string(command_code));
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
    LOG.debug(extractor->get_raw_data(), "PacketBuilder");
    throw Exceptions::NotFoundException("Event code not found in PacketBuilder registry: " + to_string(event_code));
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

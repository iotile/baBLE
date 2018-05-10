#include "PacketBuilder.hpp"

using namespace std;

// Constructors
PacketBuilder::PacketBuilder(shared_ptr<AbstractFormat> building_format) {
  m_building_format = move(building_format);
  m_output_format = nullptr;
};

PacketBuilder::PacketBuilder(shared_ptr<AbstractFormat> building_format, shared_ptr<AbstractFormat> output_format) {
m_building_format = move(building_format);
m_output_format = move(output_format);
};

// Setters
PacketBuilder& PacketBuilder::set_output_format(shared_ptr<AbstractFormat> output_format) {
  m_output_format = move(output_format);
  return *this;
};

// To build packets
unique_ptr<Packet::AbstractPacket> PacketBuilder::build(const vector<uint8_t>& raw_data) {
  uint16_t type_code = m_building_format->extract_type_code(raw_data);

  if (m_building_format->is_command(type_code)) {
    return build_command(raw_data);
  } else if (m_building_format->is_event(type_code)) {
    return build_event(type_code, raw_data);
  } else {
    throw Exceptions::NotFoundException("Given data to build a packet has no known type: " + to_string(type_code));
  }
};

unique_ptr<Packet::AbstractPacket> PacketBuilder::build_command(const vector<uint8_t>& raw_data) {
  // Extract command_code from data
  uint16_t command_code = m_building_format->extract_command_code(raw_data);

  // Get command from command_code
  auto it = m_commands.find(command_code);
  if (it == m_commands.end()) {
    throw Exceptions::NotFoundException("Command code not found in PacketBuilder registry: " + to_string(command_code));
  }

  PacketConstructor fn = it->second;
  unique_ptr<Packet::AbstractPacket> packet = fn();
  packet->import(raw_data);
  return packet;
};

unique_ptr<Packet::AbstractPacket> PacketBuilder::build_event(uint16_t event_code, const vector<uint8_t>& raw_data) {
  // Get event from event_code
  auto it = m_events.find(event_code);
  if (it == m_events.end()) {
    throw Exceptions::NotFoundException("Event code not found in PacketBuilder registry: " + to_string(event_code));
  }

  PacketConstructor fn = it->second;
  unique_ptr<Packet::AbstractPacket> packet = fn();
  packet->import(raw_data);
  return packet;
};

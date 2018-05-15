#include "PacketContainer.hpp"

using namespace std;

// Statics
map<std::tuple<Packet::Type, uint64_t>, shared_ptr<Packet::AbstractPacket>> PacketContainer::m_waiting_packets{};
map<PacketContainer::TimePoint, std::tuple<Packet::Type, uint64_t>> PacketContainer::m_expiration_waiting_packets{};

void PacketContainer::wait_response(shared_ptr<Packet::AbstractPacket> packet) {
  Packet::Type packet_type = packet->current_type();
  uint64_t uuid = packet->expected_response_uuid();

  auto key = make_tuple(packet_type, uuid);

  auto waiting_it = m_waiting_packets.find(key);
  if (waiting_it != m_waiting_packets.end()) {
    throw Exceptions::InvalidCommandException("Command already in flight.", packet->uuid_request());
  }

  m_waiting_packets.emplace(make_tuple(packet_type, uuid), move(packet));

  TimePoint inserted_time = chrono::steady_clock::now();
  m_expiration_waiting_packets.emplace(inserted_time, key);
}

void PacketContainer::expire_waiting_packets(int64_t expiration_duration) {
  if (m_expiration_waiting_packets.empty()) {
    return;
  }

  TimePoint current_time = chrono::steady_clock::now();
  auto expiration_start_time = current_time - chrono::seconds(expiration_duration);

  auto it_last_expired = m_expiration_waiting_packets.upper_bound(expiration_start_time);
  if (it_last_expired == m_expiration_waiting_packets.begin()) {
    LOG.debug("No packet to expire.", "PacketContainer");
    return;
  }

  if (it_last_expired == m_expiration_waiting_packets.end()) {
    LOG.warning("Expiring all waiting packets", "PacketContainer");
    m_waiting_packets.clear();
    m_expiration_waiting_packets.clear();
    return;
  }

  for (auto it = m_expiration_waiting_packets.begin(); it != it_last_expired; ++it) {
    LOG.warning("Expiring packet", "PacketContainer");
    m_waiting_packets.erase(it->second);
  }

  m_expiration_waiting_packets.erase(m_expiration_waiting_packets.begin(), it_last_expired);
}

// Constructors
PacketContainer::PacketContainer(shared_ptr<AbstractFormat> building_format) {
  m_building_format = move(building_format);
  m_output_format = nullptr;
}

// Setters
PacketContainer& PacketContainer::set_output_format(shared_ptr<AbstractFormat> output_format) {
  m_output_format = move(output_format);
  return *this;
}

// To build packets
shared_ptr<Packet::AbstractPacket> PacketContainer::build(const vector<uint8_t>& raw_data) {
  uint16_t type_code = m_building_format->extract_type_code(raw_data);

  if (m_building_format->is_command(type_code)) {
    return build_command(raw_data);
  } else if (m_building_format->is_event(type_code)) {
    return build_event(type_code, raw_data);
  } else {
    throw Exceptions::NotFoundException("Given data to build a packet has no known type: " + to_string(type_code));
  }
}

shared_ptr<Packet::AbstractPacket> PacketContainer::build_command(const vector<uint8_t>& raw_data) {
  // Extract command_code from data
  uint16_t command_code = m_building_format->extract_command_code(raw_data);
  uint16_t controller_id = m_building_format->extract_controller_id(raw_data);
  Packet::Type packet_type = m_building_format->packet_type();

  auto waiting_it = m_waiting_packets.find(make_tuple(packet_type, Packet::AbstractPacket::compute_uuid(command_code, controller_id)));
  if (waiting_it != m_waiting_packets.end()) {
    shared_ptr<Packet::AbstractPacket> packet = waiting_it->second;
    if (packet->on_response_received(packet_type, raw_data)) {
      //m_waiting_packets.erase(waiting_it);
      return packet;
    }
  }

  // Get command from command_code
  auto command_it = m_commands.find(command_code);
  if (command_it == m_commands.end()) {
    throw Exceptions::NotFoundException("Command code not found in PacketContainer registry: " + to_string(command_code));
  }

  PacketConstructor fn = command_it->second;
  shared_ptr<Packet::AbstractPacket> packet = fn();
  packet->import(raw_data);
  return packet;
}

shared_ptr<Packet::AbstractPacket> PacketContainer::build_event(uint16_t event_code, const vector<uint8_t>& raw_data) {
  // Get event from event_code
  auto event_it = m_events.find(event_code);
  if (event_it == m_events.end()) {
    throw Exceptions::NotFoundException("Event code not found in PacketContainer registry: " + to_string(event_code));
  }

  PacketConstructor fn = event_it->second;
  shared_ptr<Packet::AbstractPacket> packet = fn();
  packet->import(raw_data);
  return packet;
}

const std::string PacketContainer::stringify() const {
  std::stringstream result;
  result << "Commands: " << std::endl;

  for (auto& element : m_commands) {
    result << "\t" << element.first << std::endl;
  }

  result << std::endl << "Events: " << std::endl;
  for (auto& element : m_events) {
    result << "\t" << element.first << std::endl;
  }

  result << std::endl << "Packets waiting for response: " << std::endl;
  for (auto& element : m_waiting_packets) {
    result << "\tType: ";
    switch (std::get<0>(element.first)) {
      case Packet::Type::MGMT:
        result << "MGMT";
        break;
      case Packet::Type::ASCII:
        result << "ASCII";
        break;
      case Packet::Type::FLATBUFFERS:
        result << "FLATBUFFERS";
        break;
      case Packet::Type::NONE:
        result << "NONE";
        break;
    }
    result << ", Code: " << std::get<1>(element.first) << std::endl;
  }

  return result.str();
}

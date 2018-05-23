#include "PacketContainer.hpp"

using namespace std;

// Statics
unordered_map<Packet::ResponseId, shared_ptr<Packet::AbstractPacket>> PacketContainer::m_waiting_packets{};
multimap<PacketContainer::TimePoint, Packet::ResponseId> PacketContainer::m_expiration_waiting_packets{};

void PacketContainer::register_response(shared_ptr<Packet::AbstractPacket> packet) {
  vector<Packet::ResponseId> expected_uuids = packet->expected_response_ids();

  if (expected_uuids.empty()) {
    return;
  }

  TimePoint inserted_time = chrono::steady_clock::now();

  for (auto& expected_uuid : expected_uuids) {
    auto waiting_it = m_waiting_packets.find(expected_uuid);
    if (waiting_it != m_waiting_packets.end()) {
      throw Exceptions::InvalidCommandException("Command already in flight.", packet->get_uuid_request());
    }

    m_waiting_packets.emplace(expected_uuid, packet);
    m_expiration_waiting_packets.emplace(inserted_time, expected_uuid);
  }
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

    LOG.debug("Waiting packets UUID: ", "PacketContainer");
    for (auto& kv : m_expiration_waiting_packets) {
      LOG.debug("\t - " + to_string(kv.second.packet_code), "PacketContainer");
    }

    m_waiting_packets.clear();
    m_expiration_waiting_packets.clear();
    return;
  }

  for (auto it = m_expiration_waiting_packets.begin(); it != it_last_expired; ++it) {
    LOG.warning("Expiring packet (" + to_string(it->second.packet_code) + ")", "PacketContainer");
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
shared_ptr<Packet::AbstractPacket> PacketContainer::build(std::shared_ptr<AbstractExtractor> extractor) {
  uint16_t type_code = extractor->get_type_code();

  if (m_building_format->is_command(type_code)) {
    return build_command(std::move(extractor));
  } else if (m_building_format->is_event(type_code)) {
    return build_event(std::move(extractor));
  } else {
    throw Exceptions::NotFoundException("Given data to build a packet has no known type: " + to_string(type_code));
  }
}

shared_ptr<Packet::AbstractPacket> PacketContainer::build_command(std::shared_ptr<AbstractExtractor> extractor) {
  // Extract packet_code from data
  Packet::Type packet_type = m_building_format->get_packet_type();
  uint16_t command_code = extractor->get_packet_code();

  Packet::ResponseId key{
    packet_type,
    extractor->get_controller_id(),
    extractor->get_connection_id(),
    command_code
  };
  auto waiting_it = m_waiting_packets.find(key);
  if (waiting_it != m_waiting_packets.end()) {
    shared_ptr<Packet::AbstractPacket> packet = waiting_it->second;
    vector<Packet::ResponseId> expected_uuids = packet->expected_response_ids();

    if (packet->on_response_received(packet_type, extractor)) {

      for (auto& expected_uuid : expected_uuids) {
        for (auto expiration_it = m_expiration_waiting_packets.begin(); expiration_it != m_expiration_waiting_packets.end(); ++expiration_it) {
          if (expiration_it->second == expected_uuid) {
            m_expiration_waiting_packets.erase(expiration_it);
            break;
          }
        }
        m_waiting_packets.erase(expected_uuid);
      }
      return packet;
    }
  }

  // Get command from packet_code
  auto command_it = m_commands.find(command_code);
  if (command_it == m_commands.end()) {
    LOG.debug(extractor->get_raw_data(), "PacketContainer");
    throw Exceptions::NotFoundException("Command code not found in PacketContainer registry: " + to_string(command_code));
  }

  PacketConstructor fn = command_it->second;
  shared_ptr<Packet::AbstractPacket> packet = fn();
  packet->from_bytes(extractor);
  return packet;
}

shared_ptr<Packet::AbstractPacket> PacketContainer::build_event(std::shared_ptr<AbstractExtractor> extractor) {
  uint16_t event_code = extractor->get_packet_code();

  // Get event from event_code
  auto event_it = m_events.find(event_code);
  if (event_it == m_events.end()) {
    LOG.debug(extractor->get_raw_data(), "PacketContainer");
    throw Exceptions::NotFoundException("Event code not found in PacketContainer registry: " + to_string(event_code));
  }

  PacketConstructor fn = event_it->second;
  shared_ptr<Packet::AbstractPacket> packet = fn();
  packet->from_bytes(extractor);
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
    switch (element.first.packet_type) {
      case Packet::Type::MGMT:
        result << "MGMT";
        break;
      case Packet::Type::HCI:
        result << "HCI";
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
    result << ", Packet code: " << element.first.packet_code << std::endl;
  }

  return result.str();
}

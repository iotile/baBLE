#include "PacketContainer.hpp"

using namespace std;

// Statics
map<PacketContainer::RequestId, shared_ptr<Packet::AbstractPacket>> PacketContainer::m_waiting_packets{};
multimap<PacketContainer::TimePoint, PacketContainer::RequestId> PacketContainer::m_expiration_waiting_packets{};

void PacketContainer::register_response(shared_ptr<Packet::AbstractPacket> packet) {
  vector<uint64_t> expected_uuids = packet->expected_response_uuids();

  if (expected_uuids.empty()) {
    return;
  }

  Packet::Type packet_type = packet->current_type();
  TimePoint inserted_time = chrono::steady_clock::now();

  for (auto& expected_uuid : expected_uuids) {
    RequestId key = make_tuple(packet_type, expected_uuid);

    auto waiting_it = m_waiting_packets.find(key);
    if (waiting_it != m_waiting_packets.end()) {
      throw Exceptions::InvalidCommandException("Command already in flight.", packet->uuid_request());
    }

    m_waiting_packets.emplace(key, packet);
    m_expiration_waiting_packets.emplace(inserted_time, key);
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
      LOG.debug("\t - " + to_string(std::get<1>(kv.second)), "PacketContainer");
    }

    m_waiting_packets.clear();
    m_expiration_waiting_packets.clear();
    return;
  }

  for (auto it = m_expiration_waiting_packets.begin(); it != it_last_expired; ++it) {
    LOG.warning("Expiring packet (" + to_string(std::get<1>(it->second)) + ")", "PacketContainer");
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
  Packet::Type packet_type = m_building_format->packet_type();
  uint16_t command_code = extractor->get_packet_code();
  uint16_t controller_id = extractor->get_controller_id();

  RequestId key = make_tuple(packet_type, Packet::AbstractPacket::compute_uuid(controller_id, command_code));
  auto waiting_it = m_waiting_packets.find(key);
  if (waiting_it != m_waiting_packets.end()) {
    shared_ptr<Packet::AbstractPacket> packet = waiting_it->second;
    vector<uint64_t> expected_uuids = packet->expected_response_uuids();

    if (packet->on_response_received(packet_type, extractor)) {

      for (auto& expected_uuid : expected_uuids) {
        RequestId key_to_remove = make_tuple(packet_type, expected_uuid);

        for (auto expiration_it = m_expiration_waiting_packets.begin(); expiration_it != m_expiration_waiting_packets.end(); ++expiration_it) {
          if (expiration_it->second == key_to_remove) {
            m_expiration_waiting_packets.erase(expiration_it);
            break;
          }
        }
        m_waiting_packets.erase(key_to_remove);
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
    switch (std::get<0>(element.first)) {
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
    result << ", Code: " << std::get<1>(element.first) << std::endl;
  }

  return result.str();
}

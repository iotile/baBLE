#include "PacketBuilder.hpp"
#include "Log/Log.hpp"
#include "Exceptions/BaBLEException.hpp"

using namespace std;

// Constructors
PacketBuilder::PacketBuilder(shared_ptr<AbstractFormat> building_format) {
  m_building_format = move(building_format);
}

// Setters
PacketBuilder& PacketBuilder::set_ignored_packets(unordered_set<uint16_t> ignored_packets) {
  m_ignored_packets = move(ignored_packets);

  return *this;
}

// To build packets
shared_ptr<Packet::AbstractPacket> PacketBuilder::build(shared_ptr<AbstractExtractor> extractor) {
  if (!extractor->is_valid()) {
    return nullptr;
  }

  uint16_t type_code = extractor->get_type_code();

  if (m_building_format->is_command(type_code)) {
    return build_packet(move(extractor), m_commands);
  } else if (m_building_format->is_event(type_code)) {
    return build_packet(move(extractor), m_events);
  } else {
    throw Exceptions::BaBLEException(
        BaBLE::StatusCode::NotFound,
        "Given data to build a packet has no known type: " + to_string(type_code)
    );
  }
}

shared_ptr<Packet::AbstractPacket> PacketBuilder::build_packet(shared_ptr<AbstractExtractor> extractor,
                                                               const unordered_map<uint16_t, PacketConstructor>& packets) {
  // Extract packet_code from data
  uint16_t packet_code = extractor->get_packet_code();

  // Get command from packet_code
  auto it = packets.find(packet_code);
  if (it == packets.end()) {
    if (m_ignored_packets.find(packet_code) == m_ignored_packets.end()) {
      LOG.error("Packet code not found in PacketBuilder registry: " + to_string(packet_code), "PacketBuilder");
      LOG.debug(extractor->get_raw_data(), "PacketBuilder");
    } else {
      LOG.debug("Packet, in ignored list, has been ignored (" + to_string(packet_code) + ")");
    }

    return nullptr;
  }

  PacketConstructor fn = it->second;
  shared_ptr<Packet::AbstractPacket> packet = fn();
  packet->from_bytes(extractor);
  return packet;
}

const string PacketBuilder::stringify() const {
  stringstream result;
  result << "Commands: " << endl;

  for (auto& element : m_commands) {
    result << "\t" << element.first << endl;
  }

  result << endl << "Events: " << endl;
  for (auto& element : m_events) {
    result << "\t" << element.first << endl;
  }

  return result.str();
}

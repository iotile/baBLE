#include "FlatbuffersFormatExtractor.hpp"

using namespace std;

// Statics
void FlatbuffersFormatExtractor::verify(const vector<uint8_t> & data) {
  flatbuffers::Verifier packet_verifier(data.data(), data.size());
  bool packet_valid = Schemas::VerifyPacketBuffer(packet_verifier);

  if (!packet_valid) {
    throw Exceptions::WrongFormatException("Flatbuffers packet is not valid.");
  }
}

uint16_t FlatbuffersFormatExtractor::extract_payload_type(const std::vector<uint8_t>& data) {
  verify(data);

  auto fb_packet = Schemas::GetPacket(data.data());
  auto payload_type = fb_packet->payload_type();

  return static_cast<uint16_t>(payload_type);
}

uint16_t FlatbuffersFormatExtractor::extract_controller_id(const std::vector<uint8_t>& data) {
  verify(data);

  auto fb_packet = Schemas::GetPacket(data.data());
  auto controller_id = fb_packet->controller_id();

  return static_cast<uint16_t>(controller_id);
}

// Constructors
FlatbuffersFormatExtractor::FlatbuffersFormatExtractor(const vector<uint8_t> & data) {
  m_data.assign(data.begin(), data.end());
  m_packet = Schemas::GetPacket(data.data());
};

// Getters
uint16_t FlatbuffersFormatExtractor::get_controller_id() const {
  return m_packet->controller_id();
}

std::string FlatbuffersFormatExtractor::get_uuid_request() const {
  return m_packet->uuid()->str();
}

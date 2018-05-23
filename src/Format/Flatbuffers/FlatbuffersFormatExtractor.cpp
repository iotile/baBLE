#include "FlatbuffersFormatExtractor.hpp"

using namespace std;

// Statics
void FlatbuffersFormatExtractor::verify(const vector<uint8_t> & data) {
  flatbuffers::Verifier packet_verifier(data.data(), data.size());
  bool packet_valid = BaBLE::VerifyPacketBuffer(packet_verifier);

  if (!packet_valid) {
    throw Exceptions::WrongFormatException("Flatbuffers packet is not valid.");
  }
}

uint16_t FlatbuffersFormatExtractor::extract_payload_length(const vector<uint8_t>& data) {
  throw std::runtime_error("Flatbuffers format can't extract payload length: this information is not included in the format.");
}

// Constructors
FlatbuffersFormatExtractor::FlatbuffersFormatExtractor(const vector<uint8_t> & data) : AbstractExtractor(data) {
  m_packet = BaBLE::GetPacket(data.data());
  m_packet_code = static_cast<uint16_t>(m_packet->payload_type());
  m_controller_id = m_packet->controller_id();
};

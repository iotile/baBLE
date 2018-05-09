#include "FlatbuffersFormatExtractor.hpp"

// Constructors
FlatbuffersFormatExtractor::FlatbuffersFormatExtractor(const std::vector<uint8_t> & data) {
  m_data.assign(data.begin(), data.end());
  m_packet = Schemas::GetPacket(data.data());
};

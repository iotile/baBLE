#include "FlatbuffersFormatExtractor.hpp"

using namespace std;

// Constructors
FlatbuffersFormatExtractor::FlatbuffersFormatExtractor(const vector<uint8_t> & data) {
  m_data.assign(data.begin(), data.end());
  m_packet = Schemas::GetPacket(data.data());
};

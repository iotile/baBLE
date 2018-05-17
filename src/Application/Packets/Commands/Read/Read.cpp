#include "Read.hpp"

using namespace std;

namespace Packet::Commands {

  Read::Read(Packet::Type initial_type, Packet::Type translated_type)
      : CommandPacket(initial_type, translated_type) {};

  void Read::unserialize(AsciiFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
  };

  void Read::unserialize(FlatbuffersFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);
  };

  void Read::unserialize(MGMTFormatExtractor& extractor) {
    CommandPacket::unserialize(extractor);

    if (m_native_status == 0) {

    }
  };

  vector<uint8_t> Read::serialize(AsciiFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    builder
        .set_name("Read");

    return builder.build();
  };

  vector<uint8_t> Read::serialize(FlatbuffersFormatBuilder& builder) const {
    CommandPacket::serialize(builder);

    return {};
  }

  vector<uint8_t> Read::serialize(MGMTFormatBuilder& builder) const {
    CommandPacket::serialize(builder);
    return builder.build();
  };

}
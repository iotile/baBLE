#include "Ready.hpp"

using namespace std;

namespace Packet::Control {

  Ready::Ready(Packet::Type output_type)
      : AbstractPacket(output_type, output_type) {
    m_native_class = "BaBLE";
  }

  vector<uint8_t> Ready::serialize(AsciiFormatBuilder& builder) const {
    builder.set_name("Ready");
    return {};
  };

  vector<uint8_t> Ready::serialize(FlatbuffersFormatBuilder& builder) const {
    auto payload = Schemas::CreateReady(builder);

    return builder.build(payload, Schemas::Payload::Ready);
  };

}